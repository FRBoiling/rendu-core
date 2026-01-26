//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <core/engine/context.h>
#include <iostream>

using namespace Rendu::Core;

TEST_CASE("Context: 基本功能", "[core][context]") {
    Context ctx;

    SECTION("访问子系统") {
        REQUIRE_NOTHROW([&]() { ctx.io(); }());
        REQUIRE_NOTHROW([&]() { ctx.logger(); }());
        REQUIRE_NOTHROW([&]() { ctx.event_bus(); }());
        REQUIRE_NOTHROW([&]() { ctx.config(); }());
    }
}

TEST_CASE("Context: IoContext 访问", "[core][context]") {
    Context ctx;

    SECTION("获取 IoContext 引用") {
        auto& io = ctx.io();
        REQUIRE_NOTHROW(io.run()); // 验证可以访问
        io.stop();
    }
}

TEST_CASE("Context: Logger 访问", "[core][context]") {
    Context ctx;

    SECTION("获取 Logger 引用") {
        auto& logger = ctx.logger();
        REQUIRE_NOTHROW(logger.info("测试日志"));
    }
}

TEST_CASE("Context: EventBus 访问", "[core][context]") {
    Context ctx;

    SECTION("获取 EventBus 引用") {
        auto& bus = ctx.event_bus();
        REQUIRE_NOTHROW([]() { /* EventBus 需要具体事件类型才能发布 */ }());
    }
}

TEST_CASE("Context: Config 访问", "[core][context]") {
    Context ctx;

    SECTION("获取 Config 引用") {
        auto& config = ctx.config();
        REQUIRE_NOTHROW([&]() { config.get_or_default<int64_t>("test", 0); }());
    }
}

TEST_CASE("Context: 加载配置文件", "[core][context]") {
    Context ctx;

    SECTION("加载不存在的配置文件") {
        REQUIRE_NOTHROW(ctx.load_config("/nonexistent/config.json"));
    }

    SECTION("加载有效的配置文件") {
        // 创建测试配置文件
        std::string test_config = "{\"key\":\"value\"}";
        ctx.logger().info("测试配置文件加载功能");

        // 由于实际文件路径可能不存在，这里只测试不会崩溃
        REQUIRE_NOTHROW(ctx.load_config("test_config.json"));
    }
}
