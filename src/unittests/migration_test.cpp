//
// ECS Archetype 迁移测试
// 使用 Catch2 框架
//

#include "common/ecs/registry_optimized.h"
#include "common/ecs/archetype_migrator.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;

// 测试组件
struct Position {
    float x = 0.0f;
    float y = 0.0f;

    Position() = default;
    Position(float x, float y) : x(x), y(y) {}
};

struct Velocity {
    float vx = 0.0f;
    float vy = 0.0f;

    Velocity() = default;
    Velocity(float vx, float vy) : vx(vx), vy(vy) {}
};

struct Health {
    int value = 100;

    Health() = default;
    Health(int value) : value(value) {}
};

struct Name {
    std::string value;
    Name(const std::string& val = "") : value(val) {}
};

TEST_CASE("Migration - 动态添加组件", "[migration][add]") {
    RegistryOptimized registry;

    SECTION("为现有实体添加新组件") {
        Entity e = registry.create();
        Position pos(1.0f, 2.0f);
        registry.emplaceSingle<Position>(e, pos);

        REQUIRE(registry.has<Position>(e));
        REQUIRE_FALSE(registry.has<Velocity>(e));

        Velocity vel(3.0f, 4.0f);
        registry.emplaceSingle<Velocity>(e, vel);

        auto* velPtr = registry.tryGetDynamic<Velocity>(e);
        REQUIRE(velPtr != nullptr);
        REQUIRE(velPtr->vx == 3.0f);
        REQUIRE(velPtr->vy == 4.0f);
    }

    SECTION("添加多个动态组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Velocity vel(1.0f, 1.0f);
        registry.emplaceSingle<Velocity>(e, vel);

        Health hp(80);
        registry.emplaceSingle<Health>(e, hp);

        REQUIRE(registry.has<Position>(e));
        REQUIRE(registry.has<Velocity>(e));
        REQUIRE(registry.has<Health>(e));

        auto* hpPtr = registry.tryGetDynamic<Health>(e);
        REQUIRE(hpPtr != nullptr);
        REQUIRE(hpPtr->value == 80);
    }

    SECTION("批量添加动态组件") {
        std::vector<Entity> entities;
        for (int i = 0; i < 10; ++i) {
            auto e = registry.create();
            Position pos(static_cast<float>(i), 0.0f);
            registry.emplaceSingle<Position>(e, pos);
            entities.push_back(e);
        }

        for (int i = 0; i < 10; ++i) {
            Velocity vel(static_cast<float>(i), 0.0f);
            registry.emplaceSingle<Velocity>(entities[i], vel);
        }

        for (int i = 0; i < 10; ++i) {
            REQUIRE(registry.has<Velocity>(entities[i]));
        }
    }
}

TEST_CASE("Migration - 动态移除组件", "[migration][remove]") {
    RegistryOptimized registry;

    SECTION("移除动态组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Name name("TestEntity");
        registry.emplaceSingle<Name>(e, name);

        Health hp(100);
        registry.emplaceSingle<Health>(e, hp);

        REQUIRE(registry.has<Name>(e));

        registry.removeComponent<Name>(e);

        REQUIRE_FALSE(registry.has<Name>(e));
        REQUIRE(registry.has<Position>(e));
        REQUIRE(registry.has<Health>(e));
    }

    SECTION("移除不存在的组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        REQUIRE_NOTHROW(registry.removeComponent<Name>(e));
        REQUIRE_FALSE(registry.has<Name>(e));
    }

    SECTION("移除所有动态组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Name name("Test");
        registry.emplaceSingle<Name>(e, name);

        Health hp(100);
        registry.emplaceSingle<Health>(e, hp);

        registry.removeComponent<Name>(e);
        registry.removeComponent<Health>(e);

        REQUIRE_FALSE(registry.has<Name>(e));
        REQUIRE_FALSE(registry.has<Health>(e));
        REQUIRE(registry.has<Position>(e));
    }
}

TEST_CASE("Migration - 动态组件更新", "[migration][update]") {
    RegistryOptimized registry;

    SECTION("更新动态组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Velocity vel1(1.0f, 1.0f);
        registry.emplaceSingle<Velocity>(e, vel1);

        auto* velPtr = registry.tryGetDynamic<Velocity>(e);
        REQUIRE(velPtr != nullptr);
        REQUIRE(velPtr->vx == 1.0f);
        REQUIRE(velPtr->vy == 1.0f);

        Velocity vel2(5.0f, 10.0f);
        registry.emplaceSingle<Velocity>(e, vel2);

        velPtr = registry.tryGetDynamic<Velocity>(e);
        REQUIRE(velPtr != nullptr);
        REQUIRE(velPtr->vx == 5.0f);
        REQUIRE(velPtr->vy == 10.0f);
    }

    SECTION("多次更新同一组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        for (int i = 0; i < 5; ++i) {
            Velocity vel(static_cast<float>(i), static_cast<float>(i));
            registry.emplaceSingle<Velocity>(e, vel);
        }

        auto* velPtr = registry.tryGetDynamic<Velocity>(e);
        REQUIRE(velPtr != nullptr);
        REQUIRE(velPtr->vx == 4.0f);
        REQUIRE(velPtr->vy == 4.0f);
    }
}

TEST_CASE("Migration - 多个动态组件", "[migration][multiple]") {
    RegistryOptimized registry;

    SECTION("管理多个动态组件") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Velocity vel(1.0f, 1.0f);
        registry.emplaceSingle<Velocity>(e, vel);

        Health hp(100);
        registry.emplaceSingle<Health>(e, hp);

        Name name("Player1");
        registry.emplaceSingle<Name>(e, name);

        REQUIRE(registry.has<Position>(e));
        REQUIRE(registry.has<Velocity>(e));
        REQUIRE(registry.has<Health>(e));
        REQUIRE(registry.has<Name>(e));

        auto* velPtr = registry.tryGetDynamic<Velocity>(e);
        auto* hpPtr = registry.tryGetDynamic<Health>(e);
        auto* namePtr = registry.tryGetDynamic<Name>(e);

        REQUIRE(velPtr != nullptr);
        REQUIRE(hpPtr != nullptr);
        REQUIRE(namePtr != nullptr);

        REQUIRE(velPtr->vx == 1.0f);
        REQUIRE(hpPtr->value == 100);
        REQUIRE(namePtr->value == "Player1");
    }

    SECTION("选择性移除") {
        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Velocity vel(1.0f, 1.0f);
        registry.emplaceSingle<Velocity>(e, vel);

        Health hp(100);
        registry.emplaceSingle<Health>(e, hp);

        Name name("Test");
        registry.emplaceSingle<Name>(e, name);

        registry.removeComponent<Velocity>(e);

        REQUIRE_FALSE(registry.has<Velocity>(e));
        REQUIRE(registry.has<Health>(e));
        REQUIRE(registry.has<Name>(e));
    }
}

TEST_CASE("Migration - 批量操作", "[migration][batch]") {
    SECTION("批量创建实体") {
        RegistryOptimized registry;
        auto entities = registry.createBatch<Position, Velocity>(100);

        REQUIRE(entities.size() == 100);

        for (auto e : entities) {
            REQUIRE(registry.has<Position>(e));
            REQUIRE(registry.has<Velocity>(e));
        }
    }

    SECTION("批量添加动态组件") {
        RegistryOptimized registry;
        std::vector<Entity> entities;

        for (int i = 0; i < 50; ++i) {
            auto e = registry.create();
            Position pos(static_cast<float>(i), 0.0f);
            registry.emplaceSingle<Position>(e, pos);
            entities.push_back(e);
        }

        for (int i = 0; i < 50; ++i) {
            Name name("Entity" + std::to_string(i));
            registry.emplaceSingle<Name>(entities[i], name);
        }

        for (int i = 0; i < 50; ++i) {
            auto* namePtr = registry.tryGetDynamic<Name>(entities[i]);
            REQUIRE(namePtr != nullptr);
        }
    }
}

TEST_CASE("Migration - 边界情况", "[migration][edge]") {
    SECTION("已销毁实体的组件操作") {
        RegistryOptimized registry;

        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        registry.destroy(e);

        REQUIRE_NOTHROW(registry.removeComponent<Position>(e));
    }

    SECTION("重复添加相同组件") {
        RegistryOptimized registry;

        Entity e = registry.create();
        Position pos(0.0f, 0.0f);
        registry.emplaceSingle<Position>(e, pos);

        Velocity vel(1.0f, 1.0f);
        registry.emplaceSingle<Velocity>(e, vel);

        Velocity vel2(2.0f, 2.0f);
        registry.emplaceSingle<Velocity>(e, vel2);

        auto* velPtr = registry.tryGetDynamic<Velocity>(e);
        REQUIRE(velPtr != nullptr);
        REQUIRE(velPtr->vx == 2.0f);
    }
}
