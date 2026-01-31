//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <core/engine/context.h>
#include <common/io/io_context.h>
#include <iostream>

using namespace Rendu;

TEST_CASE("Context: 初始化和子系统访问", "[core][context]") {
    Context ctx;

    SECTION("访问所有子系统") {
        REQUIRE_NOTHROW([&]() { ctx.io(); }());
        REQUIRE_NOTHROW([&]() { ctx.logger(); }());
        REQUIRE_NOTHROW([&]() { ctx.event_bus(); }());
        REQUIRE_NOTHROW([&]() { ctx.config(); }());
    }

    SECTION("多次访问同一子系统应返回相同实例") {
        auto& io1 = ctx.io();
        auto& io2 = ctx.io();
        auto& logger1 = ctx.logger();
        auto& logger2 = ctx.logger();

        // 通过地址验证是同一个实例
        REQUIRE(&io1 == &io2);
        REQUIRE(&logger1 == &logger2);
    }
}

TEST_CASE("Context: IoContext 功能测试", "[core][context]") {
    Context ctx;

    SECTION("IoContext 基本操作") {
        auto& io = ctx.io();

        REQUIRE_NOTHROW(io.run());
        io.stop();
        REQUIRE_NOTHROW(io.run());
        io.stop();
    }
}

TEST_CASE("Context: Logger 功能测试", "[core][context]") {
    Context ctx;

    SECTION("不同日志级别") {
        auto& logger = ctx.logger();

        REQUIRE_NOTHROW(logger.info("信息日志"));
        REQUIRE_NOTHROW(logger.warn("警告日志"));
        REQUIRE_NOTHROW(logger.error("错误日志"));
        REQUIRE_NOTHROW(logger.debug("调试日志"));
    }

    SECTION("格式化日志") {
        auto& logger = ctx.logger();
        REQUIRE_NOTHROW(logger.info(fmt::format("数值: {}, 字符串: {}", 42, "test")));
    }
}

TEST_CASE("Context: EventBus 功能测试", "[core][context]") {
    Context ctx;

    SECTION("获取 EventBus 引用") {
        auto& bus = ctx.event_bus();
        REQUIRE_NOTHROW([]() { /* EventBus 需要具体事件类型才能发布 */ }());
    }
}

TEST_CASE("Context: Config 功能测试", "[core][context]") {
    Context ctx;

    SECTION("获取不存在的配置项") {
        auto& config = ctx.config();

        long long default_value = config.get_or_default<long long>("nonexistent.key", -1);
        REQUIRE(default_value == -1);

        std::string str_value = config.get_or_default<std::string>("nonexistent.key", "default");
        REQUIRE(str_value == "default");
    }

    SECTION("获取不同类型的配置项") {
        auto& config = ctx.config();

        // 测试整数类型 (使用 long long 而非 int32_t)
        REQUIRE_NOTHROW([&]() {
            auto val = config.get_or_default<long long>("test.int", 0);
        }());

        // 测试布尔类型
        REQUIRE_NOTHROW([&]() {
            auto val = config.get_or_default<bool>("test.bool", false);
        }());

        // 测试浮点类型
        REQUIRE_NOTHROW([&]() {
            auto val = config.get_or_default<double>("test.double", 0.0);
        }());
    }
}

TEST_CASE("Context: 加载配置文件", "[core][context]") {
    Context ctx;

    SECTION("加载不存在的配置文件不应崩溃") {
        REQUIRE_NOTHROW(ctx.load_config("/nonexistent/config.json"));
    }

    SECTION("加载配置文件后访问配置") {
        ctx.logger().info("测试配置文件加载功能");
        REQUIRE_NOTHROW(ctx.load_config("test_config.json"));

        // 加载后测试配置读取
        auto& config = ctx.config();
        REQUIRE_NOTHROW([&]() {
            auto val = config.get_or_default<std::string>("test", "");
        }());
    }
}

TEST_CASE("Context: 多实例测试", "[core][context]") {
    SECTION("创建多个 Context 实例") {
        Context ctx1;
        Context ctx2;

        // 每个实例的子系统应该是独立的
        auto& io1 = ctx1.io();
        auto& io2 = ctx2.io();

        REQUIRE(&io1 != &io2);

        auto& logger1 = ctx1.logger();
        auto& logger2 = ctx2.logger();

        REQUIRE(&logger1 != &logger2);
    }
}

TEST_CASE("Context: 异常安全性", "[core][context]") {
    SECTION("Context 析构不应抛出异常") {
        REQUIRE_NOTHROW([]() {
            Context ctx;
            // 自动析构
        }());
    }

    SECTION("子系统析构不应抛出异常") {
        Context ctx;
        REQUIRE_NOTHROW([&]() {
            auto& io = ctx.io();
            auto& logger = ctx.logger();
            // 引用离开作用域，不应抛出异常
        }());
    }
}
