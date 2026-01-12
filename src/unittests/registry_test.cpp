//
// ECS Registry 单元测试
// 使用 Catch2 框架
//

#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

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

struct Name {
    std::string value;
    Name(const std::string& val = "") : value(val) {}
};

TEST_CASE("Registry - 基础创建和销毁", "[registry][basic]") {
    RegistryOptimized registry;

    SECTION("创建实体") {
        Entity e = registry.create();
        REQUIRE(e.value() == 1); // 第一个实体 ID 为 1

        Entity e2 = registry.create();
        REQUIRE(e2.value() == 2);
    }

    SECTION("批量创建实体") {
        auto entities = registry.createBatch<Position>(10);
        REQUIRE(entities.size() == 10);

        for (size_t i = 0; i < entities.size(); ++i) {
            REQUIRE(entities[i].valid());
            REQUIRE(entities[i].index() == i + 1);
        }
    }

    SECTION("销毁实体") {
        Entity e = registry.create();
        REQUIRE(registry.valid(e));

        registry.destroy(e);
        REQUIRE_FALSE(registry.valid(e));

        // 重新创建应该复用 ID 但版本号增加
        Entity e2 = registry.create();
        REQUIRE(e2.index() == e.index()); // 复用索引
        REQUIRE(e2.version() > e.version()); // 版本号增加
    }

    SECTION("实体有效性检查") {
        Entity e = registry.create();
        REQUIRE(registry.valid(e));

        registry.destroy(e);
        REQUIRE_FALSE(registry.valid(e));

        // 零实体总是无效
        Entity null;
        REQUIRE_FALSE(registry.valid(null));
    }
}

TEST_CASE("Registry - 组件添加和获取", "[registry][component]") {
    RegistryOptimized registry;

    SECTION("emplaceSingle 添加单个组件") {
        Entity e = registry.create();

        auto& pos = registry.emplaceSingle<Position>(e, Position{10.0f, 20.0f});
        REQUIRE(pos.x == 10.0f);
        REQUIRE(pos.y == 20.0f);
    }

    SECTION("emplace 添加多个组件") {
        Entity e = registry.create();

        registry.emplace<Position, Velocity>(
            e,
            Position{1.0f, 2.0f},
            Velocity{0.5f, 0.5f}
        );

        auto components = registry.get<Position, Velocity>(e);
        auto& pos = std::get<0>(components);
        auto& vel = std::get<1>(components);

        REQUIRE(pos.x == 1.0f);
        REQUIRE(pos.y == 2.0f);
        REQUIRE(vel.vx == 0.5f);
        REQUIRE(vel.vy == 0.5f);
    }

    SECTION("get 获取组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{5.0f, 10.0f});

        auto& pos = std::get<0>(registry.get<Position>(e));
        REQUIRE(pos.x == 5.0f);
        REQUIRE(pos.y == 10.0f);
    }

    SECTION("has 检查组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e);

        REQUIRE(registry.has<Position>(e));
        REQUIRE_FALSE(registry.has<Velocity>(e));
        REQUIRE_FALSE(registry.has<Health>(e));
    }
}

TEST_CASE("Registry - 视图遍历", "[registry][view]") {
    RegistryOptimized registry;

    SECTION("基础视图") {
        std::vector<Entity> entities;
        for (int i = 0; i < 10; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
            entities.push_back(e);
        }

        int count = 0;
        auto view = registry.view<Position>();
        view.each([&](Entity e, Position& p) {
            count++;
            REQUIRE(p.x >= 0.0f);
        });

        REQUIRE(count == 10);
    }

    SECTION("多组件视图") {
        std::vector<Entity> entities;
        for (int i = 0; i < 5; ++i) {
            auto e = registry.create();
            registry.emplace<Position, Velocity>(
                e,
                Position{static_cast<float>(i), 0.0f},
                Velocity{static_cast<float>(i) * 0.1f, 0.0f}
            );
            entities.push_back(e);
        }

        int count = 0;
        auto view = registry.view<Position, Velocity>();
        view.each([&](Entity e, Position& p, Velocity& v) {
            count++;
        });

        REQUIRE(count == 5);
    }
}

TEST_CASE("Registry - 分组", "[registry][group]") {
    RegistryOptimized registry;

    SECTION("添加实体到分组") {
        auto& group = registry.getGroup<Position>();

        std::vector<Entity> entities;
        for (int i = 0; i < 5; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0.0f});
            group.add(e);
            entities.push_back(e);
        }

        REQUIRE(group.size() == 5);
    }

    SECTION("遍历分组") {
        auto& group = registry.getGroup<Position>();

        std::vector<Entity> entities;
        for (int i = 0; i < 3; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), static_cast<float>(i * 2)});
            group.add(e);
            entities.push_back(e);
        }

        int count = 0;
        group.each(registry, [&](Entity e, Position& p) {
            count++;
            REQUIRE(p.x * 2 == p.y);
        });

        REQUIRE(count == 3);
    }

    SECTION("移除分组中的实体") {
        auto& group = registry.getGroup<Position>();

        std::vector<Entity> entities;
        for (int i = 0; i < 3; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0, 0});
            group.add(e);
            entities.push_back(e);
        }

        REQUIRE(group.size() == 3);

        group.remove(entities[1]);
        REQUIRE(group.size() == 2);
    }
}

TEST_CASE("Registry - 快照和恢复", "[registry][snapshot]") {
    RegistryOptimized registry;

    SECTION("创建快照") {
        auto e1 = registry.create();
        auto e2 = registry.create();

        auto snapshot = registry.createSnapshot();
        REQUIRE(snapshot.nextEntityId == 3);
    }

    SECTION("恢复快照") {
        auto snapshot = registry.createSnapshot();

        // 创建一些实体
        auto e1 = registry.create();
        auto e2 = registry.create();

        REQUIRE(e2.value() == 2);

        // 恢复快照
        registry.restoreFromSnapshot(snapshot);

        // 新创建的实体应该从恢复后的 ID 开始
        auto e3 = registry.create();
        REQUIRE(e3.value() == 1); // 回到快照状态
    }
}

TEST_CASE("Registry - 动态组件管理", "[registry][dynamic]") {
    RegistryOptimized registry;

    SECTION("动态添加组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{0, 0});

        // 动态添加第二个组件
        auto& name = registry.emplaceSingle<Name>(e);
        name.value = "TestEntity";

        REQUIRE(name.value == "TestEntity");
        REQUIRE(registry.has<Name>(e));
    }

    SECTION("更新已存在的组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{1.0f, 2.0f});

        // 更新组件
        Position newPos{10.0f, 20.0f};
        registry.emplaceSingle<Position>(e, newPos);

        auto& pos = std::get<0>(registry.get<Position>(e));
        REQUIRE(pos.x == 10.0f);
        REQUIRE(pos.y == 20.0f);
    }

    SECTION("移除组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{0, 0});
        registry.emplaceSingle<Name>(e);

        REQUIRE(registry.has<Name>(e));

        registry.removeComponent<Name>(e);
        REQUIRE_FALSE(registry.has<Name>(e));
    }
}

TEST_CASE("Registry - 边界情况", "[registry][edge]") {
    RegistryOptimized registry;

    SECTION("获取不存在实体的组件") {
        Entity e = registry.create();
        registry.destroy(e);

        REQUIRE_THROWS(registry.get<Position>(e));
    }

    SECTION("获取实体不存在的组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e);

        REQUIRE_THROWS(registry.get<Velocity>(e));
    }

    SECTION("重复添加相同组件") {
        Entity e = registry.create();
        registry.emplaceSingle<Position>(e, Position{1.0f, 2.0f});

        // 应该更新而不是报错
        registry.emplaceSingle<Position>(e, Position{3.0f, 4.0f});
        auto& pos = std::get<0>(registry.get<Position>(e));
        REQUIRE(pos.x == 3.0f);
        REQUIRE(pos.y == 4.0f);
    }
}
