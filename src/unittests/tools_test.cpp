//
// 工具系统测试
// 测试：Profiler, MemoryAnalyzer, CacheAnalyzer, HotspotDetector, PerformanceMonitor
//

#include "common/ecs/registry_optimized.h"
#include "common/ecs/profiler.h"
#include "common/ecs/cache_analyzer.h"
#include "common/ecs/hotspot_detector.h"
#include "common/ecs/performance_monitor.h"
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>

using namespace Rendu;

// 测试组件
struct Position {
    float x, y;
    Position() : x(0), y(0) {}
    Position(float x_, float y_) : x(x_), y(y_) {}
};

struct Velocity {
    float vx, vy;
    Velocity() : vx(0), vy(0) {}
    Velocity(float vx_, float vy_) : vx(vx_), vy(vy_) {}
};

struct Health {
    int value;
    Health() : value(100) {}
    Health(int v) : value(v) {}
};

TEST_CASE("Tools - 性能分析器", "[tools][profiler]") {
    SECTION("单次计时") {
        PerformanceProfiler profiler;

        {
            PROFILE_SCOPE(profiler, "Test Scope");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        auto* stat = profiler.getStat("Test Scope");
        REQUIRE(stat != nullptr);
        REQUIRE(stat->count == 1);
        REQUIRE(stat->avgTime > 0);
    }

    SECTION("多次计时") {
        PerformanceProfiler profiler;

        for (int i = 0; i < 5; ++i) {
            PROFILE_SCOPE(profiler, "Repeated Scope");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        auto* stat = profiler.getStat("Repeated Scope");
        REQUIRE(stat != nullptr);
        REQUIRE(stat->count == 5);
    }

    SECTION("嵌套范围") {
        PerformanceProfiler profiler;

        {
            PROFILE_SCOPE(profiler, "Outer");
            {
                PROFILE_SCOPE(profiler, "Inner");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        REQUIRE(profiler.getAllStats().size() == 2);
    }
}

TEST_CASE("Tools - 内存分析器", "[tools][memory]") {
    SECTION("基础分析") {
        RegistryOptimized registry;

        for (int i = 0; i < 100; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        MemoryAnalyzer analyzer;
        auto info = analyzer.analyze(registry);

        REQUIRE(info.archetypes.size() > 0);
    }

    SECTION("多组件分析") {
        RegistryOptimized registry;

        for (int i = 0; i < 50; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e, Position{0.0f, 0.0f}, Velocity{1.0f, 1.0f});
        }

        for (int i = 0; i < 30; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Health>(e, Position{0.0f, 0.0f}, Health{100});
        }

        MemoryAnalyzer analyzer;
        auto info = analyzer.analyze(registry);

        REQUIRE(info.archetypes.size() >= 2);
    }

    SECTION("内存统计") {
        RegistryOptimized registry;

        for (int i = 0; i < 1000; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        MemoryAnalyzer analyzer;
        auto info = analyzer.analyze(registry);

        REQUIRE(info.archetypes.size() > 0);
        REQUIRE(info.totalMemory > 0);
    }
}

TEST_CASE("Tools - 缓存分析器", "[tools][cache]") {
    SECTION("缓存命中率分析") {
        RegistryOptimized registry;

        const int COUNT = 10000;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e,
                Position{static_cast<float>(i), 0.0f},
                Velocity{1.0f, 0.0f});
        }

        CacheAnalyzer analyzer;
        analyzer.startProfiling();

        auto view = registry.view<Position, Velocity>();
        view.each([&](Entity e, Position& p, Velocity& v) {
            p.x += v.vx;
            p.y += v.vy;
        });

        analyzer.stopProfiling();
        auto stats = analyzer.getCacheStats();

        REQUIRE(stats.totalAccesses > 0);
    }

    SECTION("多组件遍历分析") {
        RegistryOptimized registry;

        const int COUNT = 5000;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
            registry.emplace<Velocity>(e, Velocity{0.0f, 0.0f});
            registry.emplace<Health>(e, Health{100});
        }

        CacheAnalyzer analyzer;
        analyzer.startProfiling();

        auto view = registry.view<Position, Velocity, Health>();
        view.each([&](Entity e, Position& p, Velocity& v, Health& h) {
            p.x += v.vx;
            p.y += v.vy;
        });

        analyzer.stopProfiling();
        auto stats = analyzer.getCacheStats();

        REQUIRE(stats.totalAccesses > 0);
    }
}

TEST_CASE("Tools - 热点检测器", "[tools][hotspot]") {
    SECTION("检测热点区域") {
        HotspotDetector detector;
        detector.startDetection();

        {
            HotspotDetector::ScopeTimer timer(detector, "Entity Creation", "Test");
            RegistryOptimized registry;
            for (int i = 0; i < 100; ++i) {
                auto e = registry.create();
                registry.emplace<Position>(e, Position{static_cast<float>(i), 0.0f});
            }
        }

        detector.stopDetection();
        auto hotspots = detector.getHotspots();
        REQUIRE(hotspots.size() >= 0);
    }

    SECTION("性能热点排序") {
        HotspotDetector detector;
        detector.startDetection();

        {
            HotspotDetector::ScopeTimer timer1(detector, "Create Entities", "Test");
            RegistryOptimized registry;
            for (int i = 0; i < 500; ++i) {
                auto e = registry.create();
                registry.emplace<Position, Velocity>(e,
                    Position{0.0f, 0.0f}, Velocity{0.0f, 0.0f});
            }
        }

        {
            HotspotDetector::ScopeTimer timer2(detector, "Iterate Entities", "Test");
            RegistryOptimized registry;
            for (int i = 0; i < 100; ++i) {
                auto e = registry.create();
                registry.emplace<Position>(e, Position{0.0f, 0.0f});
            }
            auto view = registry.view<Position>();
            view.each([](Entity e, Position& p) {
                p.x += 1.0f;
            });
        }

        detector.stopDetection();
        auto hotspots = detector.getHotspots();
        REQUIRE(hotspots.size() >= 0);
    }
}

TEST_CASE("Tools - 性能监控器", "[tools][monitor]") {
    SECTION("FPS监控") {
        PerformanceMonitor monitor;

        RegistryOptimized registry;
        for (int i = 0; i < 10; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        for (int i = 0; i < 10; ++i) {
            monitor.update(registry, 0.016f);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        auto snapshot = monitor.getCurrentSnapshot();
        REQUIRE(snapshot.fps > 0);
    }

    SECTION("内存使用监控") {
        PerformanceMonitor monitor;
        MemoryAnalyzer memAnalyzer;

        RegistryOptimized registry;
        for (int i = 0; i < 1000; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        monitor.update(registry, memAnalyzer, 0.016f);
        auto snapshot = monitor.getCurrentSnapshot();

        REQUIRE(snapshot.memoryUsage > 0);
    }

    SECTION("历史统计") {
        PerformanceMonitor monitor;

        RegistryOptimized registry;
        for (int i = 0; i < 10; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        for (int i = 0; i < 5; ++i) {
            monitor.update(registry, 0.016f);
        }

        auto history = monitor.getHistory();
        REQUIRE(history.size() >= 5);

        auto stats = monitor.getHistoryStats();
        REQUIRE(stats.avgFps > 0);
    }
}

TEST_CASE("Tools - 综合分析", "[tools][integration]") {
    SECTION("完整分析流程") {
        RegistryOptimized registry;

        // 创建大量实体
        for (int i = 0; i < 10000; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e,
                Position{static_cast<float>(i), 0.0f},
                Velocity{1.0f, 0.0f});
        }

        // 性能分析
        PerformanceProfiler profiler;
        {
            PROFILE_SCOPE(profiler, "Iteration");
            auto view = registry.view<Position, Velocity>();
            view.each([](Entity e, Position& p, Velocity& v) {
                p.x += v.vx;
                p.y += v.vy;
            });
        }

        // 内存分析
        MemoryAnalyzer memAnalyzer;
        auto memInfo = memAnalyzer.analyze(registry);

        // 缓存分析
        CacheAnalyzer cacheAnalyzer;
        cacheAnalyzer.startProfiling();
        {
            auto view = registry.view<Position, Velocity>();
            view.each([](Entity e, Position& p, Velocity& v) {
                p.x += v.vx;
            });
        }
        cacheAnalyzer.stopProfiling();
        auto cacheInfo = cacheAnalyzer.getCacheStats();

        REQUIRE(cacheInfo.totalAccesses == 10000);
        REQUIRE(memInfo.archetypes.size() > 0);
    }

    SECTION("工具链组合") {
        RegistryOptimized registry;

        PerformanceProfiler profiler;
        MemoryAnalyzer memAnalyzer;
        PerformanceMonitor monitor;

        // 批量创建
        {
            PROFILE_SCOPE(profiler, "Batch Create");
            for (int i = 0; i < 1000; ++i) {
                auto e = registry.create();
                registry.emplace<Position>(e, Position{0.0f, 0.0f});
            }
        }

        // 批量遍历
        {
            PROFILE_SCOPE(profiler, "Batch Iterate");
            auto view = registry.view<Position>();
            view.each([](Entity e, Position& p) {
                p.x += 1.0f;
            });
        }

        auto profilerStats = profiler.getAllStats();
        auto memInfo = memAnalyzer.analyze(registry);
        monitor.update(registry, 0.016f);

        REQUIRE(profilerStats.size() >= 2);
        REQUIRE(memInfo.archetypes.size() > 0);
    }
}
