#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>
#include <atomic>

#include "core/actor/message_router.h"
#include "core/actor/message_router_events.h"
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

TEST_CASE("MessageRouter 事件系统", "[actor][message_router][events]") {
    io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    MessageRouter router(io);

    auto actor1 = ActorRef("/system/actor1", 1);
    auto sender = ActorRef("/system/sender", 100);

    SECTION("路由添加事件") {
        std::atomic<int> event_count{0};

        auto sub_id = router.event_bus().subscribe<actor::RouteAddedEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::RouteAddedEvent* ae = dynamic_cast<const actor::RouteAddedEvent*>(&e);
                if (ae) {
                    REQUIRE(ae->rule_name == "test_route");
                    REQUIRE(ae->priority == 10);
                    event_count++;
                }
            }
        );

        router.add_route("test_route",
                        [](const ActorRef&, const std::string&) { return true; },
                        actor1, 10);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    SECTION("消息路由事件") {
        std::atomic<int> event_count{0};

        auto sub_id = router.event_bus().subscribe<actor::MessageRoutedEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::MessageRoutedEvent* me = dynamic_cast<const actor::MessageRoutedEvent*>(&e);
                if (me) {
                    REQUIRE(me->message_type == "test_message");
                    REQUIRE(me->success);
                    REQUIRE(me->duration_ms >= 0);
                    event_count++;
                }
            }
        );

        router.add_route("test_route",
                        [](const ActorRef&, const std::string& msg) {
                            return msg == "test_message";
                        },
                        actor1, 10);

        router.route(sender, "test_message");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    SECTION("路由异常事件") {
        std::atomic<int> event_count{0};

        auto sub_id = router.event_bus().subscribe<actor::RouteExceptionEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::RouteExceptionEvent* re = dynamic_cast<const actor::RouteExceptionEvent*>(&e);
                if (re) {
                    REQUIRE(re->error_message == "test exception");
                    event_count++;
                }
            }
        );

        router.add_route("exception_route",
                        [](const ActorRef&, const std::string&) {
                            throw std::runtime_error("test exception");
                            return true;
                        },
                        actor1);

        router.route(sender, "test");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    SECTION("批量路由事件") {
        std::atomic<int> event_count{0};

        auto sub_id = router.event_bus().subscribe<actor::BatchRoutedEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::BatchRoutedEvent* be = dynamic_cast<const actor::BatchRoutedEvent*>(&e);
                if (be) {
                    REQUIRE(be->message_type == "test");
                    REQUIRE(be->matched_count > 0);
                    event_count++;
                }
            }
        );

        router.add_route("route1", [](auto&, auto&) { return true; }, actor1);
        router.add_route("route2", [](auto&, auto&) { return true; }, actor1);

        router.route_all(sender, "test");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    SECTION("广播事件") {
        std::atomic<int> event_count{0};

        auto sub_id = router.event_bus().subscribe<actor::BroadcastEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::BroadcastEvent* be = dynamic_cast<const actor::BroadcastEvent*>(&e);
                if (be) {
                    REQUIRE(be->message_type == "broadcast_msg");
                    REQUIRE(be->target_count == 3);
                    event_count++;
                }
            }
        );

        router.add_route("route1", [](auto&, auto&) { return true; }, actor1);
        router.add_route("route2", [](auto&, auto&) { return true; }, actor1);
        router.add_route("route3", [](auto&, auto&) { return true; }, actor1);

        router.broadcast("broadcast_msg");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    SECTION("异步事件发布") {
        std::atomic<int> event_count{0};

        auto sub_id = router.event_bus().subscribe<actor::RouteAddedEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::RouteAddedEvent* ae = dynamic_cast<const actor::RouteAddedEvent*>(&e);
                if (ae) {
                    event_count++;
                }
            }
        );

        // 异步发布
        router.event_bus().publish_async(actor::RouteAddedEvent("async_route", 5, actor1));

        // 等待异步事件处理
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    SECTION("延迟事件发布") {
        std::atomic<int> event_count{0};
        auto start = std::chrono::steady_clock::now();

        auto sub_id = router.event_bus().subscribe<actor::RouteAddedEvent>(
            [&](const Rendu::event::Event& e) {
                const actor::RouteAddedEvent* ae = dynamic_cast<const actor::RouteAddedEvent*>(&e);
                if (ae) {
                    event_count++;
                    auto end = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    REQUIRE(elapsed >= 50);  // 至少延迟了50ms
                }
            }
        );

        // 延迟发布
        router.event_bus().publish_delayed(actor::RouteAddedEvent("delayed_route", 5, actor1), 50);

        // 等待延迟事件处理
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(event_count == 1);
        router.event_bus().unsubscribe(sub_id);
    }

    io.stop();
}

TEST_CASE("MessageRouter 指标收集", "[actor][message_router][metrics]") {
    Rendu::io::IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    SECTION("启用指标收集") {
        MessageRouter router(io, true);  // 启用指标

        auto actor1 = ActorRef("/system/actor1", 1);
        auto sender = ActorRef("/system/sender", 100);

        // 添加路由
        router.add_route("route1", [](auto&, auto&) { return true; }, actor1);
        router.add_route("route2", [](auto&, auto&) { return true; }, actor1);

        // 路由消息
        router.route(sender, "test");
        router.route(sender, "test");

        // 批量路由
        router.route_all(sender, "test");

        // 广播
        router.broadcast("broadcast");

        // 等待指标收集
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // 获取指标（验证格式）
        auto metrics = router.get_metrics();
        REQUIRE_FALSE(metrics.empty());
        REQUIRE(metrics.find("message_router") != std::string::npos);
    }

    SECTION("禁用指标收集") {
        MessageRouter router(io, false);  // 禁用指标

        auto actor1 = ActorRef("/system/actor1", 1);
        auto sender = ActorRef("/system/sender", 100);

        router.add_route("route1", [](auto&, auto&) { return true; }, actor1);
        router.route(sender, "test");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // 指标应该为空或很少
        auto metrics = router.get_metrics();
        REQUIRE_FALSE(router.metrics_enabled());
    }

    io.stop();
}
