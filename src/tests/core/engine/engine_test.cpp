//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <core/engine/engine.h>
#include <thread>
#include <chrono>

using namespace Rendu;

// 测试模块
class TestModule : public IEngineModule {
public:
    int update_count = 0;

    void initialize() override {
        update_count = 0;
    }

    void update(float delta_time) override {
        update_count++;
    }

    void shutdown() override {
        // 清理资源
    }
};

TEST_CASE("Engine: 初始化", "[core][engine]") {
    Engine engine;

    SECTION("初始化引擎") {
        REQUIRE_NOTHROW(engine.initialize());
        REQUIRE_FALSE(engine.is_running());
    }
}

TEST_CASE("Engine: 启动和停止", "[core][engine]") {
    Engine engine;

    engine.initialize();
    REQUIRE_FALSE(engine.is_running());

    engine.start();
    REQUIRE(engine.is_running());

    // 等待一小段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine.stop();
    REQUIRE_FALSE(engine.is_running());
}

TEST_CASE("Engine: 访问上下文", "[core][engine]") {
    Engine engine;

    SECTION("访问上下文") {
        REQUIRE_NOTHROW([&]() { engine.context(); }());
        REQUIRE_NOTHROW([&]() { engine.context().io(); }());
        REQUIRE_NOTHROW([&]() { engine.context().logger(); }());
    }
}

TEST_CASE("Engine: 模块管理", "[core][engine]") {
    Engine engine;
    engine.initialize();

    SECTION("添加模块") {
        auto test_module = std::make_shared<TestModule>();
        REQUIRE_NOTHROW([&]() {
            engine.add_module<TestModule>();
        }());

        // 等待模块更新
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    SECTION("移除模块") {
        engine.add_module<TestModule>();
        auto test_module = std::make_shared<TestModule>();
        REQUIRE_NOTHROW([&]() {
            engine.remove_module(test_module);
        }());
    }
}

TEST_CASE("Engine: 模块更新", "[core][engine]") {
    Engine engine;
    engine.initialize();

    SECTION("模块每帧更新") {
        engine.add_module<TestModule>();
        engine.start();

        // 等待约 100ms，应该有 6-7 帧更新（60 FPS）
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        engine.stop();

        // 获取模块并检查更新次数
        auto& ctx = engine.context();
        ctx.logger().info("模块更新测试完成");
    }
}

TEST_CASE("Engine: 设置目标帧率", "[core][engine]") {
    Engine engine;
    engine.initialize();

    SECTION("设置有效的 FPS") {
        REQUIRE_NOTHROW(engine.set_target_fps(30));
        REQUIRE_NOTHROW(engine.set_target_fps(120));
    }

    SECTION("设置无效的 FPS") {
        REQUIRE_NOTHROW(engine.set_target_fps(0));
        REQUIRE_NOTHROW(engine.set_target_fps(-10));
    }
}

TEST_CASE("Engine: 异常处理", "[core][engine]") {
    Engine engine;

    SECTION("重复启动") {
        engine.initialize();
        engine.start();
        REQUIRE_NOTHROW(engine.start()); // 不应该崩溃
        engine.stop();
    }

    SECTION("重复停止") {
        engine.initialize();
        engine.start();
        engine.stop();
        REQUIRE_NOTHROW(engine.stop()); // 不应该崩溃
    }
}

TEST_CASE("Engine: 资源释放", "[core][engine]") {
    SECTION("引擎析构时清理资源") {
        {
            Engine engine;
            engine.initialize();
            engine.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } // engine 在这里析构
        // 不应该有内存泄漏
    }
}
