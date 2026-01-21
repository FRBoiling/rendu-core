//
// ECS 信号/观察者系统单元测试
//
// 测试标签说明:
// [signal][construct] - on_construct 事件测试
// [signal][update]    - onUpdate 事件测试
// [signal][destroy]   - onDestroy 事件测试
// [signal][raii]      - RAII 连接管理测试
// [signal][chain]     - 信号链式操作测试
// [signal][performance] - 性能测试
// [signal][edge]      - 边界情况测试
// [signal][integration] - 完整流程集成测试
//

#include "common/ecs/registry_optimized.h"
#include "common/events/events.h"
#include <catch2/catch_test_macros.hpp>

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

struct Health {
    int value;
    Health(int value = 100) : value(value) {}
};

TEST_CASE("Signal - on_construct 事件", "[signal][construct]") {
    RegistryOptimized registry;

    SECTION("基础构造事件") {
        int constructCount = 0;
        float lastX = 0;

        auto conn = registry.onConstruct<Position>([&](Entity e, Position& pos) {
            constructCount++;
            lastX = pos.x;
        });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{5.0f, 0, 0});

        REQUIRE(constructCount == 1);
        REQUIRE(lastX == 5.0f);
    }

    SECTION("多个组件的构造事件") {
        int posCount = 0;
        int velCount = 0;
        int healthCount = 0;

        auto conn1 = registry.onConstruct<Position>([&](Entity e, Position& pos) { posCount++; });
        auto conn2 = registry.onConstruct<Velocity>([&](Entity e, Velocity& vel) { velCount++; });
        auto conn3 = registry.onConstruct<Health>([&](Entity e, Health& health) { healthCount++; });

        Entity e = registry.create();
        registry.emplace<Position, Velocity, Health>(
            e,
            Position{0, 0, 0},
            Velocity{0, 0, 0},
            Health{100}
        );

        REQUIRE(posCount == 1);
        REQUIRE(velCount == 1);
        REQUIRE(healthCount == 1);
    }

    SECTION("多个监听器") {
        int count1 = 0;
        int count2 = 0;

        auto conn1 = registry.onConstruct<Position>([&](Entity e, Position& pos) { count1++; });
        auto conn2 = registry.onConstruct<Position>([&](Entity e, Position& pos) { count2++; });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{0, 0, 0});

        REQUIRE(count1 == 1);
        REQUIRE(count2 == 1);
    }
}

TEST_CASE("Signal - onUpdate 事件", "[signal][update]") {
    RegistryOptimized registry;

    SECTION("基础更新事件") {
        int updateCount = 0;
        float lastX = 0;

        auto conn = registry.onUpdate<Position>([&](Entity e, Position& pos) {
            updateCount++;
            lastX = pos.x;
        });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{1.0f, 0, 0}); // 构造，不触发 update
        REQUIRE(updateCount == 0);

        registry.emplaceSingle<Position>(e, Position{10.0f, 0, 0}); // 更新，触发 update
        REQUIRE(updateCount == 1);
        REQUIRE(lastX == 10.0f);
    }

    SECTION("多次更新") {
        int updateCount = 0;

        auto conn = registry.onUpdate<Position>([&](Entity e, Position& pos) {
            updateCount++;
        });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{0, 0, 0});

        // 多次更新
        for (int i = 0; i < 5; ++i) {
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0, 0});
        }

        REQUIRE(updateCount == 5);
    }
}

TEST_CASE("Signal - onDestroy 事件", "[signal][destroy]") {
    RegistryOptimized registry;

    SECTION("基础销毁事件") {
        int destroyCount = 0;
        Entity destroyedEntity;

        auto conn = registry.onDestroy<Position>([&](Entity e, Position& pos) {
            destroyCount++;
            destroyedEntity = e;
        });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{1.0f, 2.0f, 3.0f});

        registry.destroyWithSignals<Position>(e);

        REQUIRE(destroyCount == 1);
        REQUIRE(destroyedEntity.value() == e.value());
    }

    SECTION("多个组件的销毁事件") {
        int posCount = 0;
        int velCount = 0;

        auto conn1 = registry.onDestroy<Position>([&](Entity e, Position& pos) { posCount++; });
        auto conn2 = registry.onDestroy<Velocity>([&](Entity e, Velocity& vel) { velCount++; });

        Entity e = registry.create();
        registry.emplace<Position, Velocity>(e, Position{0, 0, 0}, Velocity{0, 0, 0});

        registry.destroyWithSignals<Position, Velocity>(e);

        REQUIRE(posCount == 1);
        REQUIRE(velCount == 1);
    }
}

TEST_CASE("Signal - RAII 连接管理", "[signal][raii]") {
    RegistryOptimized registry;

    SECTION("自动断开连接") {
        int count = 0;

        {
            Connection<void(Entity, Position&)> conn =
                registry.onConstruct<Position>([&](Entity e, Position& pos) {
                    count++;
                });

            Entity e1 = registry.create();
            registry.emplaceSingle<Position>(e1, Position{0, 0, 0});
            REQUIRE(count == 1);

        } // conn 自动断开

        Entity e2 = registry.create();
        registry.emplaceSingle<Position>(e2, Position{1, 2, 3});
        REQUIRE(count == 1); // 还是 1，说明连接已断开
    }

    SECTION("手动断开连接") {
        int count = 0;

        auto conn = registry.onConstruct<Position>([&](Entity e, Position& pos) {
            count++;
        });

        Entity e1 = registry.create();
        registry.emplaceSingle<Position>(e1, Position{0, 0, 0});
        REQUIRE(count == 1);

        conn.disconnect();

        Entity e2 = registry.create();
        registry.emplaceSingle<Position>(e2, Position{1, 2, 3});
        REQUIRE(count == 1); // 还是 1
    }

    SECTION("连接有效性检查") {
        auto conn = registry.onConstruct<Position>([&](Entity e, Position& pos) {});

        REQUIRE(conn.valid());

        conn.disconnect();
        REQUIRE_FALSE(conn.valid());
    }
}

TEST_CASE("Signal - 信号链式操作", "[signal][chain]") {
    RegistryOptimized registry;

    SECTION("监听器链") {
        std::vector<std::string> logs;

        auto conn1 = registry.onConstruct<Position>([&](Entity e, Position& pos) {
            logs.push_back("LOG: Position created for " + std::to_string(e.index()));
        });

        auto conn2 = registry.onConstruct<Position>([&](Entity e, Position& pos) {
            if (pos.x < 0) {
                logs.push_back("WARN: Negative position!");
            }
        });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{-1.0f, 0, 0});

        REQUIRE(logs.size() == 2);
        REQUIRE(logs[0].find("LOG") != std::string::npos);
        REQUIRE(logs[1].find("WARN") != std::string::npos);
    }
}

TEST_CASE("Signal - 性能测试", "[signal][performance]") {
    RegistryOptimized registry;

    SECTION("大量实体创建") {
        auto conn = registry.onConstruct<Position>([&](Entity e, Position& pos) {});

        constexpr int COUNT = 1000;
        auto entities = registry.createBatch<Position>(COUNT);

        REQUIRE(entities.size() == COUNT);
    }

    SECTION("多次更新") {
        int updateCount = 0;

        auto conn = registry.onUpdate<Position>([&](Entity e, Position& pos) {
            updateCount++;
        });

        constexpr int COUNT = 100;
        constexpr int UPDATES = 10;

        std::vector<Entity> entities;
        for (int i = 0; i < COUNT; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0, 0, 0});
            entities.push_back(e);
        }

        // 批量更新
        for (int i = 0; i < UPDATES; ++i) {
            for (auto e : entities) {
                registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0, 0});
            }
        }

        REQUIRE(updateCount == COUNT * UPDATES);
    }
}

TEST_CASE("Signal - 边界情况", "[signal][edge]") {
    RegistryOptimized registry;

    SECTION("断开连接后再使用") {
        int count = 0;

        auto conn = registry.onConstruct<Position>([&](Entity e, Position& pos) {
            count++;
        });

        conn.disconnect();

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{0, 0, 0});

        REQUIRE(count == 0);
    }

    SECTION("多次断开连接") {
        auto conn = registry.onConstruct<Position>([&](Entity e, Position& pos) {});

        REQUIRE_NOTHROW(conn.disconnect());
        REQUIRE_NOTHROW(conn.disconnect()); // 不应该报错
    }

    SECTION("空监听器") {
        // 不应该崩溃
        auto conn = registry.onConstruct<Position>([](Entity e, Position& pos) {});
        conn.disconnect();
    }
}

TEST_CASE("Signal - 完整流程", "[signal][integration]") {
    RegistryOptimized registry;

    SECTION("创建 -> 更新 -> 销毁") {
        std::vector<std::string> events;

        auto conn1 = registry.onConstruct<Position>([&](Entity e, Position& pos) {
            events.emplace_back("construct");
        });

        auto conn2 = registry.onUpdate<Position>([&](Entity e, Position& pos) {
            events.emplace_back("update");
        });

        auto conn3 = registry.onDestroy<Position>([&](Entity e, Position& pos) {
            events.emplace_back("destroy");
        });

        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{1, 2, 3}); // construct
        REQUIRE(events.back() == "construct");

        registry.emplaceSingle<Position>(e, Position{10, 20, 30}); // update
        REQUIRE(events.back() == "update");

        registry.destroyWithSignals<Position>(e); // destroy
        REQUIRE(events.back() == "destroy");

        REQUIRE(events.size() == 3);
    }
}
