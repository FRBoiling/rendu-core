//
// ECS 性能基准测试
// 使用 Catch2 框架
//

#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <chrono>

using namespace Rendu;

// 测试组件
struct Position {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Velocity {
    float vx, vy;
    Velocity(float vx = 0, float vy = 0) : vx(vx), vy(vy) {}
};

struct Health {
    int value;
    Health(int value = 100) : value(value) {}
};

TEST_CASE("Performance - 批量创建实体", "[performance][create]") {
    RegistryOptimized registry;

    SECTION("创建 1000 个实体") {
        const int COUNT = 1000;

        auto entities = registry.createBatch<Position>(COUNT);

        REQUIRE(entities.size() == COUNT);
    }

    SECTION("创建 10000 个实体") {
        RegistryOptimized registry;
        const int COUNT = 10000;

        auto entities = registry.createBatch<Position>(COUNT);

        REQUIRE(entities.size() == COUNT);
    }

    SECTION("创建多种组件实体") {
        const int COUNT = 1000;

        auto entities = registry.createBatch<Position, Velocity, Health>(COUNT);

        REQUIRE(entities.size() == COUNT);

        for (auto e : entities) {
            REQUIRE(registry.has<Position>(e));
            REQUIRE(registry.has<Velocity>(e));
            REQUIRE(registry.has<Health>(e));
        }
    }
}

TEST_CASE("Performance - 视图遍历", "[performance][view]") {
    RegistryOptimized registry;

    SECTION("单组件视图遍历") {
        const int COUNT = 10000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
        }

        auto view = registry.view<Position>();
        int count = 0;

        view.each([&](Entity e, Position& p) {
            count++;
            p.x += 1.0f;
        });

        REQUIRE(count == COUNT);
    }

    SECTION("多组件视图遍历") {
        const int COUNT = 5000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(
                e,
                Position{static_cast<float>(i), 0.0f},
                Velocity{static_cast<float>(i) * 0.1f, 0.0f}
            );
        }

        auto view = registry.view<Position, Velocity>();
        int count = 0;

        view.each([&](Entity e, Position& p, Velocity& v) {
            count++;
            p.x += v.vx;
            p.y += v.vy;
        });

        REQUIRE(count == COUNT);
    }

    SECTION("排除视图遍历") {
        const int COUNT = 2000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            if (i % 2 == 0) {
                registry.emplace<Position, Velocity>(
                    e,
                    Position{static_cast<float>(i), 0.0f},
                    Velocity{0.0f, 0.0f}
                );
            } else {
                registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
            }
        }

        auto view = registry.view<Position>();
        int count = 0;

        view.each([&](Entity e, Position& p) {
            count++;
        });

        REQUIRE(count == COUNT);
    }
}

TEST_CASE("Performance - 分组操作", "[performance][group]") {
    RegistryOptimized registry;

    SECTION("分组遍历") {
        const int COUNT = 5000;
        auto& group = registry.getGroup<Position>();

        std::vector<Entity> entities;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), static_cast<float>(i * 2)});
            group.add(e);
            entities.push_back(e);
        }

        int count = 0;
        group.each(registry, [&](Entity e, Position& p) {
            count++;
            p.x += 1.0f;
            p.y += 2.0f;
        });

        REQUIRE(count == COUNT);
    }

    SECTION("分组添加和移除") {
        auto& group = registry.getGroup<Position>();

        std::vector<Entity> entities;
        for (int i = 0; i < 1000; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0.0f, 0.0f});
            group.add(e);
            entities.push_back(e);
        }

        REQUIRE(group.size() == 1000);

        for (int i = 0; i < 500; ++i) {
            group.remove(entities[i]);
        }

        REQUIRE(group.size() == 500);
    }
}

TEST_CASE("Performance - 动态组件操作", "[performance][dynamic]") {
    RegistryOptimized registry;

    SECTION("动态添加组件") {
        const int COUNT = 1000;
        std::vector<Entity> entities;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0.0f, 0.0f});
            entities.push_back(e);
        }

        for (int i = 0; i < COUNT; ++i) {
            Velocity vel(static_cast<float>(i), 0.0f);
            registry.emplaceSingle<Velocity>(entities[i], vel);
        }

        for (int i = 0; i < COUNT; ++i) {
            REQUIRE(registry.has<Velocity>(entities[i]));
        }
    }

    SECTION("动态更新组件") {
        const int COUNT = 1000;
        std::vector<Entity> entities;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0.0f, 0.0f});
            Velocity vel(0.0f, 0.0f);
            registry.emplaceSingle<Velocity>(e, vel);
            entities.push_back(e);
        }

        for (int i = 0; i < COUNT; ++i) {
            Velocity vel(static_cast<float>(i), static_cast<float>(i));
            registry.emplaceSingle<Velocity>(entities[i], vel);
        }
    }

    SECTION("动态移除组件") {
        const int COUNT = 1000;
        std::vector<Entity> entities;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0.0f, 0.0f});
            Velocity vel(0.0f, 0.0f);
            registry.emplaceSingle<Velocity>(e, vel);
            entities.push_back(e);
        }

        for (int i = 0; i < COUNT; ++i) {
            registry.removeComponent<Velocity>(entities[i]);
        }

        for (int i = 0; i < COUNT; ++i) {
            REQUIRE_FALSE(registry.has<Velocity>(entities[i]));
        }
    }
}

TEST_CASE("Performance - 实体销毁", "[performance][destroy]") {
    RegistryOptimized registry;

    SECTION("批量销毁实体") {
        const int COUNT = 5000;
        std::vector<Entity> entities;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
            entities.push_back(e);
        }

        for (auto e : entities) {
            registry.destroy(e);
        }

        auto view = registry.view<Position>();
        int count = 0;
        view.each([&](Entity e, Position& p) { count++; });

        REQUIRE(count == 0);
    }
}

TEST_CASE("Performance - 快照操作", "[performance][snapshot]") {
    RegistryOptimized registry;

    SECTION("创建快照") {
        const int COUNT = 1000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
        }

        auto snapshot = registry.createSnapshot();
        REQUIRE(snapshot.nextEntityId == COUNT + 1);
    }

    SECTION("恢复快照") {
        const int COUNT = 1000;

        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
        }

        auto snapshot = registry.createSnapshot();

        for (int i = 0; i < 100; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0.0f, 0.0f});
        }

        registry.restoreFromSnapshot(snapshot);

        auto e = registry.create();
        REQUIRE(e.value() == COUNT + 1);
    }
}

TEST_CASE("Performance - 综合场景", "[performance][integration]") {
    SECTION("游戏循环模拟") {
        RegistryOptimized registry;

        const int ENTITY_COUNT = 1000;
        const int FRAME_COUNT = 100;

        std::vector<Entity> entities;

        // 创建实体
        for (int i = 0; i < ENTITY_COUNT; ++i) {
            auto e = registry.create();
            if (i % 3 == 0) {
                registry.emplace<Position, Velocity>(
                    e,
                    Position{0.0f, 0.0f},
                    Velocity{1.0f, 1.0f}
                );
            } else {
                registry.emplaceSingle<Position>(e, Position{0.0f, 0.0f});
            }
            entities.push_back(e);
        }

        // 模拟游戏帧
        for (int frame = 0; frame < FRAME_COUNT; ++frame) {
            auto view = registry.view<Position, Velocity>();
            view.each([](Entity e, Position& p, Velocity& v) {
                p.x += v.vx;
                p.y += v.vy;
            });

            // 每隔 10 帧添加一些动态组件
            if (frame % 10 == 0 && frame < 50) {
                for (int i = 0; i < 10; ++i) {
                    Health hp(100);
                    registry.emplaceSingle<Health>(entities[i], hp);
                }
            }
        }
    }
}
