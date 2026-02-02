#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <thread>
#include <chrono>
#include <memory>

#include "core/actor/message_router.h"
#include "core/actor/actor_ref.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"


using namespace Rendu;

// 全局 Logger 初始化
struct LoggerSetup {
    Rendu::io::IoContext io;
    std::thread io_thread;

    LoggerSetup() : io(1) {
        Rendu::log::init_default_io_context(io);
        io_thread = std::thread([this]() { io.run(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ~LoggerSetup() {
        io.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }
};

static LoggerSetup g_logger_setup;

TEST_CASE("MessageRouter 基本构造", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    SECTION("默认构造") {
        Rendu::MessageRouter router(io);
        REQUIRE(router.get_route_count() == 0);
        REQUIRE(router.get_all_routes().empty());
        REQUIRE(router.metrics_enabled());
    }

    SECTION("禁用指标构造") {
        Rendu::MessageRouter router(io, false);
        REQUIRE(router.get_route_count() == 0);
        REQUIRE(router.get_all_routes().empty());
        REQUIRE_FALSE(router.metrics_enabled());
    }
}

TEST_CASE("MessageRouter 添加路由规则", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);

    SECTION("添加单个路由") {
        bool added = router.add_route(
            "route1",
            [](const ActorRef&, const std::string& msg_type) {
                return msg_type == "test";
            },
            actor1,
            10
        );

        REQUIRE(added);
        REQUIRE(router.get_route_count() == 1);
        REQUIRE(router.has_route("route1"));
        REQUIRE(router.is_route_enabled("route1"));
    }

    SECTION("添加多个路由") {
        REQUIRE(router.add_route("route1", [](auto&, auto&) { return true; },
                                actor1, 10));
        REQUIRE(router.add_route("route2", [](auto&, auto&) { return true; },
                                actor2, 5));
        REQUIRE(router.get_route_count() == 2);
    }

    SECTION("重复添加相同名称的路由") {
        REQUIRE(router.add_route("route1", [](auto&, auto&) { return true; },
                                actor1));
        bool added = router.add_route("route1", [](auto&, auto&) { return true; },
                                     actor2);

        REQUIRE_FALSE(added);
        REQUIRE(router.get_route_count() == 1);
    }
}

TEST_CASE("MessageRouter 路由消息", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto sender = Rendu::ActorRef("/system/sender", 100);

    SECTION("匹配路由") {
        router.add_route(
            "test_route",
            [](const ActorRef&, const std::string& msg_type) {
                return msg_type == "test_message";
            },
            actor1,
            10
        );

        auto result = router.route(sender, "test_message");

        REQUIRE(result.matched);
        REQUIRE(result.rule_name == "test_route");
        REQUIRE(result.target.path() == actor1.path());
    }

    SECTION("不匹配任何路由") {
        router.add_route(
            "test_route",
            [](const ActorRef&, const std::string& msg_type) {
                return msg_type == "test_message";
            },
            actor1
        );

        auto result = router.route(sender, "other_message");

        REQUIRE_FALSE(result.matched);
        REQUIRE(result.rule_name.empty());
    }

    SECTION("优先级路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1, 5);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2, 10);

        auto result = router.route(sender, "test");

        REQUIRE(result.matched);
        REQUIRE(result.rule_name == "route2"); // 优先级更高的路由
        REQUIRE(result.target.path() == actor2.path());
    }
}

TEST_CASE("MessageRouter 批量路由", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto actor3 = Rendu::ActorRef("/system/actor3", 3);
    auto sender = Rendu::ActorRef("/system/sender", 100);

    SECTION("路由到所有匹配的目标") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);
        router.add_route("route3", [](auto&, const std::string& msg) {
                return msg != "skip";
            },
            actor3);

        auto results = router.route_all(sender, "test");

        REQUIRE(results.size() == 3);
        REQUIRE(results[0].matched);
        REQUIRE(results[1].matched);
        REQUIRE(results[2].matched);
    }

    SECTION("部分路由匹配") {
        router.add_route("route1", [](auto&, const std::string& msg) {
                return msg == "match";
            },
            actor1);
        router.add_route("route2", [](auto&, auto&) { return false; },
                        actor2);

        auto results = router.route_all(sender, "match");

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].matched);
        REQUIRE(results[0].target.path() == actor1.path());
    }
}

TEST_CASE("MessageRouter 广播消息", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto actor3 = Rendu::ActorRef("/system/actor3", 3);

    SECTION("广播到所有启用的目标") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);
        router.add_route("route3", [](auto&, auto&) { return true; },
                        actor3);

        auto targets = router.broadcast("test");

        REQUIRE(targets.size() == 3);
    }

    SECTION("排除禁用的路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);
        router.add_route("route3", [](auto&, auto&) { return true; },
                        actor3);

        router.disable_route("route2");

        auto targets = router.broadcast("test");

        REQUIRE(targets.size() == 2);
    }
}

TEST_CASE("MessageRouter 启用/禁用路由", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto sender = Rendu::ActorRef("/system/sender", 3);

    SECTION("禁用路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1, 5);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2, 10);

        REQUIRE(router.is_route_enabled("route1"));

        router.disable_route("route1");
        REQUIRE_FALSE(router.is_route_enabled("route1"));

        auto result = router.route(sender, "test");
        REQUIRE(result.matched);
        REQUIRE(result.rule_name == "route2"); // 跳过 route1
    }

    SECTION("启用路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);

        router.disable_route("route1");
        REQUIRE_FALSE(router.is_route_enabled("route1"));

        router.enable_route("route1");
        REQUIRE(router.is_route_enabled("route1"));
    }

    SECTION("启用不存在的路由") {
        REQUIRE_FALSE(router.enable_route("non_existent"));
    }

    SECTION("禁用不存在的路由") {
        REQUIRE_FALSE(router.disable_route("non_existent"));
    }
}

TEST_CASE("MessageRouter 移除路由", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);

    SECTION("移除存在的路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        REQUIRE(router.get_route_count() == 1);

        bool removed = router.remove_route("route1");
        REQUIRE(removed);
        REQUIRE(router.get_route_count() == 0);
        REQUIRE_FALSE(router.has_route("route1"));
    }

    SECTION("移除不存在的路由") {
        REQUIRE_FALSE(router.remove_route("non_existent"));
    }

    SECTION("移除路由后不影响其他路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);

        router.remove_route("route1");
        REQUIRE(router.get_route_count() == 1);
        REQUIRE(router.has_route("route2"));
    }
}

TEST_CASE("MessageRouter 路由优先级", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto actor3 = Rendu::ActorRef("/system/actor3", 3);
    auto sender = Rendu::ActorRef("/system/sender", 4);

    SECTION("设置路由优先级") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1, 5);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2, 10);

        router.set_route_priority("route1", 15);

        auto result = router.route(sender, "test");
        REQUIRE(result.matched);
        REQUIRE(result.rule_name == "route1"); // 更高的优先级
    }

    SECTION("负优先级") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1, -10);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2, 5);

        auto result = router.route(sender, "test");
        REQUIRE(result.matched);
        REQUIRE(result.rule_name == "route2");
    }
}

TEST_CASE("MessageRouter 路由统计", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto sender = Rendu::ActorRef("/system/sender", 3);

    SECTION("基本统计") {
        router.add_route("route1", [](auto&, const std::string& msg) {
                return msg == "match";
            },
            actor1);
        router.add_route("route2", [](auto&, auto&) { return false; },
                        actor2);

        router.route(sender, "match");
        router.route(sender, "no_match");
        router.route(sender, "match");

        auto stats = router.get_stats();
        REQUIRE(stats.total_routes == 3);
        REQUIRE(stats.matched_routes == 2);
        REQUIRE(stats.unmatched_routes == 1);
        REQUIRE(stats.disabled_routes == 0);
    }

    SECTION("禁用路由统计") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);

        router.disable_route("route1");
        router.route_all(sender, "test");

        auto stats = router.get_stats();
        REQUIRE(stats.disabled_routes > 0);
    }

    SECTION("重置统计") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.route(sender, "test");

        auto stats_before = router.get_stats();
        REQUIRE(stats_before.total_routes > 0);

        router.reset_stats();

        auto stats_after = router.get_stats();
        REQUIRE(stats_after.total_routes == 0);
        REQUIRE(stats_after.matched_routes == 0);
        REQUIRE(stats_after.unmatched_routes == 0);
    }
}

TEST_CASE("MessageRouter 边界情况", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto sender = Rendu::ActorRef("/system/sender", 2);

    SECTION("空路由器路由") {
        auto result = router.route(sender, "test");
        REQUIRE_FALSE(result.matched);
        REQUIRE(result.rule_name.empty());
    }

    SECTION("路由规则抛出异常") {
        router.add_route("exception_route",
                        [](const ActorRef&, const std::string&) {
                            throw std::runtime_error("test exception");
                            return true;
                        },
                        actor1);

        // 不应该崩溃，应该跳过异常规则
        REQUIRE_NOTHROW(router.route(sender, "test"));
    }

    SECTION("获取不存在的路由") {
        REQUIRE_FALSE(router.get_route("non_existent").has_value());
    }
}

TEST_CASE("MessageRouter 清空路由", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);

    SECTION("清空所有路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2);

        REQUIRE(router.get_route_count() == 2);

        router.clear();

        REQUIRE(router.get_route_count() == 0);
        REQUIRE(router.get_all_routes().empty());
    }
}

TEST_CASE("MessageRouter 线程安全", "[actor][message_router]") {
    // 使用禁用日志的版本，避免 IoContext 初始化问题
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io, false);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);
    auto sender = Rendu::ActorRef("/system/sender", 3);

    SECTION("并发添加路由") {
        const int num_threads = 5;
        const int routes_per_thread = 10;
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&router, &actor1, t, routes_per_thread]() {
                for (int i = 0; i < routes_per_thread; ++i) {
                    std::string route_name = "route_" + std::to_string(t) + "_" + std::to_string(i);
                    router.add_route(
                        route_name,
                        [](const ActorRef&, const std::string&) { return true; },
                        actor1,
                        i
                    );
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(router.get_route_count() == num_threads * routes_per_thread);
    }

    SECTION("并发路由消息") {
        // 预先添加一些路由
        for (int i = 0; i < 5; ++i) {
            router.add_route(
                "route_" + std::to_string(i),
                [i](const ActorRef&, const std::string& msg) {
                    return msg == "test";
                },
                actor1,
                i
            );
        }

        const int num_threads = 10;
        const int routes_per_thread = 100;
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&router, &sender, routes_per_thread]() {
                for (int i = 0; i < routes_per_thread; ++i) {
                    router.route(sender, "test");
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        auto stats = router.get_stats();
        REQUIRE(stats.total_routes == num_threads * routes_per_thread);
    }
}

TEST_CASE("MessageRouter 带日志的线程安全", "[actor][message_router]") {

    SECTION("并发添加路由（带日志）") {
        // 使用启用日志的版本
        Rendu::io::IoContext io(1);
        std::thread([&io]() { io.run(); }).detach();
        Rendu::MessageRouter router(io, true);
        auto actor1 = Rendu::ActorRef("/system/actor1", 1);

        const int num_threads = 3;
        const int routes_per_thread = 5;
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&router, &actor1, t, routes_per_thread]() {
                for (int i = 0; i < routes_per_thread; ++i) {
                    std::string route_name = "logged_route_" + std::to_string(t) + "_" + std::to_string(i);
                    router.add_route(
                        route_name,
                        [](const ActorRef&, const std::string&) { return true; },
                        actor1,
                        i
                    );
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(router.get_route_count() == num_threads * routes_per_thread);
    }

    SECTION("并发路由消息（带日志）") {
        // 使用启用日志的版本
        Rendu::io::IoContext io(1);
        std::thread([&io]() { io.run(); }).detach();
        Rendu::MessageRouter router(io, true);
        auto actor1 = Rendu::ActorRef("/system/actor1", 1);
        auto sender = Rendu::ActorRef("/system/sender", 2);

        for (int i = 0; i < 3; ++i) {
            router.add_route(
                "logged_route_" + std::to_string(i),
                [](const ActorRef&, const std::string& msg) {
                    return msg == "test";
                },
                actor1,
                i
            );
        }

        const int num_threads = 5;
        const int routes_per_thread = 20;
        std::vector<std::thread> threads;

        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&router, &sender, routes_per_thread]() {
                for (int i = 0; i < routes_per_thread; ++i) {
                    router.route(sender, "test");
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        auto stats = router.get_stats();
        REQUIRE(stats.total_routes == num_threads * routes_per_thread);
    }
}

TEST_CASE("MessageRouter 获取所有路由", "[actor][message_router]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();
    Rendu::MessageRouter router(io);

    auto actor1 = Rendu::ActorRef("/system/actor1", 1);
    auto actor2 = Rendu::ActorRef("/system/actor2", 2);

    SECTION("获取所有路由") {
        router.add_route("route1", [](auto&, auto&) { return true; },
                        actor1, 10);
        router.add_route("route2", [](auto&, auto&) { return true; },
                        actor2, 5);

        auto routes = router.get_all_routes();
        REQUIRE(routes.size() == 2);

        // 由于 unordered_map 的顺序不确定，使用 find 检查
        bool found_route1 = false, found_route2 = false;
        for (const auto& route : routes) {
            if (route.rule_name == "route1") found_route1 = true;
            if (route.rule_name == "route2") found_route2 = true;
        }
        REQUIRE(found_route1);
        REQUIRE(found_route2);
    }
}
