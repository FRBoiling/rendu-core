//
// 深度性能分析测试
// 可选运行，用于详细的性能分析和对比
//

#include "common/ecs/registry_optimized.h"
#include "common/ecs/profiler.h"
#include "common/ecs/cache_analyzer.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <vector>
#include <chrono>

using namespace Rendu;
// 测试组件
struct Position {
    float x, y, z;
    Position(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Velocity {
    float vx, vy, vz;
    Velocity(float vx = 0, float vy = 0, float vz = 0) : vx(vx), vy(vy), vz(vz) {}
};

struct Rotation {
    float x, y, z, w;
    Rotation(float x = 0, float y = 0, float z = 0, float w = 1) : x(x), y(y), z(z), w(w) {}
};

struct Scale {
    float x, y, z;
    Scale(float x = 1, float y = 1, float z = 1) : x(x), y(y), z(z) {}
};

struct Health {
    int value;
    Health(int value = 100) : value(value) {}
};

TEST_CASE("Profiling - 大规模实体创建", "[profiling][create]") {
    SECTION("创建10000个实体") {
        RegistryOptimized registry;
        const int COUNT = 10000;

        BENCHMARK("Create 10000 entities with Position") {
            for (int i = 0; i < COUNT; ++i) {
                auto e = registry.create();
                registry.emplace<Position>(e, Position{
                    static_cast<float>(i),
                    static_cast<float>(i),
                    0.0f
                });
            }
            return registry.size();
        };
    }

    SECTION("创建多组件实体") {
        RegistryOptimized registry;
        const int COUNT = 5000;

        BENCHMARK("Create 5000 entities with 3 components") {
            for (int i = 0; i < COUNT; ++i) {
                auto e = registry.create();
                registry.emplace<Position, Velocity, Rotation>(e,
                    Position{0.0f, 0.0f, 0.0f},
                    Velocity{0.0f, 0.0f, 0.0f},
                    Rotation{0.0f, 0.0f, 0.0f, 1.0f}
                );
            }
            return registry.size();
        };
    }
}

TEST_CASE("Profiling - 多组件遍历性能", "[profiling][iterate]") {
    SECTION("双组件遍历") {
        RegistryOptimized registry;
        const int COUNT = 10000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e,
                Position{static_cast<float>(i), 0.0f, 0.0f},
                Velocity{1.0f, 0.0f, 0.0f}
            );
        }

        BENCHMARK("Iterate 10000 entities (2 components)") {
            auto view = registry.view<Position, Velocity>();
            int count = 0;
            view.each([&count](Entity e, Position& p, Velocity& v) {
                p.x += v.vx;
                p.y += v.vy;
                p.z += v.vz;
                count++;
            });
            return count;
        };
    }

    SECTION("三组件遍历") {
        RegistryOptimized registry;
        const int COUNT = 10000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity, Rotation>(e,
                Position{0.0f, 0.0f, 0.0f},
                Velocity{0.0f, 0.0f, 0.0f},
                Rotation{0.0f, 0.0f, 0.0f, 1.0f}
            );
        }

        BENCHMARK("Iterate 10000 entities (3 components)") {
            auto view = registry.view<Position, Velocity, Rotation>();
            int count = 0;
            view.each([&count](Entity e, Position& p, Velocity& v, Rotation& r) {
                p.x += v.vx;
                r.w += 0.001f;
                count++;
            });
            return count;
        };
    }

    SECTION("四组件遍历") {
        RegistryOptimized registry;
        const int COUNT = 10000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity, Rotation, Scale>(e,
                Position{0.0f, 0.0f, 0.0f},
                Velocity{0.0f, 0.0f, 0.0f},
                Rotation{0.0f, 0.0f, 0.0f, 1.0f},
                Scale{1.0f, 1.0f, 1.0f}
            );
        }

        BENCHMARK("Iterate 10000 entities (4 components)") {
            auto view = registry.view<Position, Velocity, Rotation, Scale>();
            int count = 0;
            view.each([&count](Entity e, Position& p, Velocity& v, Rotation& r, Scale& s) {
                p.x += v.vx;
                s.x += 0.001f;
                count++;
            });
            return count;
        };
    }
}

TEST_CASE("Profiling - 动态组件操作", "[profiling][dynamic]") {
    SECTION("批量添加组件") {
        RegistryOptimized registry;
        const int COUNT = 5000;

        std::vector<Entity> entities;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f, 0.0f});
            entities.push_back(e);
        }

        BENCHMARK("Add Velocity to 5000 entities") {
            for (auto e : entities) {
                registry.emplace<Velocity>(e, Velocity{1.0f, 0.0f, 0.0f});
            }
            return registry.size();
        };
    }

    SECTION("批量移除组件") {
        RegistryOptimized registry;
        const int COUNT = 5000;

        std::vector<Entity> entities;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f, 0.0f});
            registry.emplace<Velocity>(e, Velocity{0.0f, 0.0f, 0.0f});
            entities.push_back(e);
        }

        BENCHMARK("Remove Velocity from 5000 entities") {
            for (auto e : entities) {
                registry.removeComponent<Velocity>(e);
            }
            return registry.size();
        };
    }
}

TEST_CASE("Profiling - 内存和缓存分析", "[profiling][memory]") {
    SECTION("内存使用分析") {
        RegistryOptimized registry;

        for (int i = 0; i < 10000; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e,
                Position{0.0f, 0.0f, 0.0f},
                Velocity{0.0f, 0.0f, 0.0f}
            );
        }

        MemoryAnalyzer analyzer;
        auto info = analyzer.analyze(registry);

        REQUIRE(info.archetypes.size() > 0);
    }

    SECTION("缓存效率分析") {
        RegistryOptimized registry;
        CacheAnalyzer analyzer;
        
        // 设置缓存分析器到注册表
        registry.setCacheAnalyzer(&analyzer);

        const int COUNT = 10000;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(e,
                Position{static_cast<float>(i), 0.0f, 0.0f},
                Velocity{1.0f, 0.0f, 0.0f}
            );
        }

        analyzer.startProfiling();
        {
            auto view = registry.view<Position, Velocity>();
            view.each([](Entity e, Position& p, Velocity& v) {
                p.x += v.vx;
            });
        }
        analyzer.stopProfiling();
        auto info = analyzer.getCacheStats();

        REQUIRE(info.totalAccesses > 0); // 现在应该自动记录访问
    }
}

TEST_CASE("Profiling - 综合场景", "[profiling][integration]") {
    SECTION("游戏循环模拟") {
        RegistryOptimized registry;
        PerformanceProfiler profiler;

        const int ENTITY_COUNT = 5000;

        // 创建阶段
        {
            PROFILE_SCOPE(profiler, "Create Entities");
            for (int i = 0; i < ENTITY_COUNT; ++i) {
                auto e = registry.create();
                registry.emplace<Position, Velocity, Health>(e,
                    Position{0.0f, 0.0f, 0.0f},
                    Velocity{0.0f, 0.0f, 0.0f},
                    Health{100}
                );
            }
        }

        // 更新阶段
        {
            PROFILE_SCOPE(profiler, "Update Entities");
            auto view = registry.view<Position, Velocity>();
            view.each([](Entity e, Position& p, Velocity& v) {
                p.x += v.vx;
                p.y += v.vy;
                p.z += v.vz;
            });
        }

        // 碰撞检测阶段
        {
            PROFILE_SCOPE(profiler, "Collision Detection");
            auto view = registry.view<Position, Health>();
            int processed = 0;
            view.each([&processed](Entity e, Position& p, Health& h) {
                if (p.x > 100.0f) {
                    h.value -= 1;
                }
                processed++;
            });
        }

        auto stats = profiler.getAllStats();
        REQUIRE(stats.size() == 3);
    }

    SECTION("混合操作性能") {
        RegistryOptimized registry;
        const int COUNT = 1000;

        std::vector<Entity> entities;

        BENCHMARK("Mixed operations (create + iterate + destroy)") {
            // 创建
            for (int i = 0; i < COUNT; ++i) {
                auto e = registry.create();
                registry.emplace<Position>(e, Position{0.0f, 0.0f, 0.0f});
                entities.push_back(e);
            }

            // 遍历
            auto view = registry.view<Position>();
            view.each([](Entity e, Position& p) {
                p.x += 1.0f;
            });

            // 销毁一半
            for (size_t i = 0; i < entities.size() / 2; ++i) {
                registry.destroy(entities[i]);
            }

            return registry.size();
        };
    }
}
