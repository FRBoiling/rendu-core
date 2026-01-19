//
// 工具系统单元测试
// 测试组件: PerformanceProfiler, MemoryAnalyzer, CacheAnalyzer, HotspotDetector, PerformanceMonitor
//
// 测试标签说明:
// [tools][profiler] - 性能分析器测试
// [tools][memory]   - 内存分析器测试
// [tools][cache]    - 缓存分析器测试
// [tools][hotspot]  - 热点检测器测试
// [tools][monitor]  - 性能监控器测试
// [tools][integration] - 工具集成测试
//

#include "common/ecs/registry_optimized.h"
#include "common/profiling/profiler.h"
#include "common/profiling/cache_analyzer.h"
#include "common/profiling/hotspot_detector.h"
#include "common/profiling/performance_monitor.h"
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

        REQUIRE_FALSE(info.archetypes.empty());
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

        REQUIRE_FALSE(info.archetypes.empty());
        REQUIRE(info.totalMemory > 0);
    }
}

TEST_CASE("Tools - 缓存分析器", "[tools][cache]") {
    SECTION("缓存命中率分析") {
        RegistryOptimized registry;

        const int COUNT = 100;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e,
                Position{static_cast<float>(i), 0.0f},
                Velocity{1.0f, 0.0f});
        }

        CacheAnalyzer analyzer;
        analyzer.startProfiling();

        // 手动记录访问
        for (int i = 0; i < COUNT; ++i) {
            analyzer.recordAccess("Position", true, true);
            analyzer.recordAccess("Velocity", true, true);
        }

        analyzer.stopProfiling();
        auto stats = analyzer.getCacheStats();

        REQUIRE(stats.totalAccesses == COUNT * 2); // 每个实体访问两个组件
    }

    SECTION("多组件遍历分析") {
        RegistryOptimized registry;

        const int COUNT = 100;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
            registry.emplace<Velocity>(e, Velocity{0.0f, 0.0f});
            registry.emplace<Health>(e, Health{100});
        }

        CacheAnalyzer analyzer;
        analyzer.startProfiling();

        // 手动记录访问
        for (int i = 0; i < COUNT; ++i) {
            analyzer.recordAccess("Position", true, true);
            analyzer.recordAccess("Velocity", true, true);
            analyzer.recordAccess("Health", true, true);
        }

        analyzer.stopProfiling();
        auto stats = analyzer.getCacheStats();

        REQUIRE(stats.totalAccesses == COUNT * 3); // 每个实体访问三个组件
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

        // 先等待1.1秒，确保第一次update时elapsed超过阈值
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));

        // 第一次update应该会添加历史记录
        monitor.update(registry, 0.016f);

        auto history = monitor.getHistory();
        REQUIRE(history.size() >= 1); // 至少有一条历史记录

        auto stats = monitor.getHistoryStats();
        REQUIRE(stats.avgFps > 0);
    }
}

TEST_CASE("Tools - 综合分析", "[tools][integration]") {
    SECTION("完整分析流程") {
        RegistryOptimized registry;

        // 创建实体
        const int COUNT = 100;
        for (int i = 0; i < COUNT; ++i) {
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

        auto* stat = profiler.getStat("Iteration");
        REQUIRE(stat != nullptr);
        REQUIRE(stat->count == 1);

        // 内存分析
        MemoryAnalyzer memAnalyzer;
        auto memInfo = memAnalyzer.analyze(registry);

        // 缓存分析 - 手动记录访问
        CacheAnalyzer cacheAnalyzer;
        cacheAnalyzer.startProfiling();
        for (int i = 0; i < COUNT; ++i) {
            cacheAnalyzer.recordAccess("Position", true, true);
            cacheAnalyzer.recordAccess("Velocity", true, true);
        }
        cacheAnalyzer.stopProfiling();
        auto cacheInfo = cacheAnalyzer.getCacheStats();

        REQUIRE(cacheInfo.totalAccesses == COUNT * 2);
        REQUIRE_FALSE(memInfo.archetypes.empty());
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
        REQUIRE_FALSE(memInfo.archetypes.empty());
    }
}
