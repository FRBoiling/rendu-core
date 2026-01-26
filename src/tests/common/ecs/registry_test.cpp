//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <common/ecs/registry.h>
#include <string>

using namespace Rendu::ecs;

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

TEST_CASE("Registry: 创建和销毁实体", "[ecs][registry]") {
    Registry registry;
    
    SECTION("创建实体") {
        Entity entity = registry.create();
        REQUIRE(registry.valid(entity));
        REQUIRE(registry.size() == 1);
    }
    
    SECTION("销毁实体") {
        Entity entity = registry.create();
        REQUIRE(registry.valid(entity));
        
        registry.destroy(entity);
        REQUIRE(!registry.valid(entity));
    }
    
    SECTION("创建多个实体") {
        Entity e1 = registry.create();
        Entity e2 = registry.create();
        Entity e3 = registry.create();
        
        REQUIRE(registry.valid(e1));
        REQUIRE(registry.valid(e2));
        REQUIRE(registry.valid(e3));
        REQUIRE(registry.size() == 3);
    }
}

TEST_CASE("Registry: 组件操作", "[ecs][registry]") {
    Registry registry;
    
    SECTION("添加组件") {
        Entity entity = registry.create();
        registry.emplace<Position>(entity, 1.0f, 2.0f);
        
        REQUIRE(registry.has<Position>(entity));
        auto& pos = registry.get<Position>(entity);
        REQUIRE(pos.x == 1.0f);
        REQUIRE(pos.y == 2.0f);
    }
    
    SECTION("添加多个组件") {
        Entity entity = registry.create();
        registry.emplace<Position>(entity, 1.0f, 2.0f);
        registry.emplace<Velocity>(entity, 3.0f, 4.0f);
        
        REQUIRE(registry.has<Position>(entity));
        REQUIRE(registry.has<Velocity>(entity));
        
        auto& pos = registry.get<Position>(entity);
        auto& vel = registry.get<Velocity>(entity);
        
        REQUIRE(pos.x == 1.0f);
        REQUIRE(vel.vx == 3.0f);
    }
    
    SECTION("移除组件") {
        Entity entity = registry.create();
        registry.emplace<Position>(entity, 1.0f, 2.0f);
        REQUIRE(registry.has<Position>(entity));
        
        registry.remove<Position>(entity);
        REQUIRE(!registry.has<Position>(entity));
    }
    
    SECTION("修改组件") {
        Entity entity = registry.create();
        registry.emplace<Position>(entity, 1.0f, 2.0f);
        
        auto& pos = registry.get<Position>(entity);
        pos.x = 5.0f;
        pos.y = 6.0f;
        
        auto& pos2 = registry.get<Position>(entity);
        REQUIRE(pos2.x == 5.0f);
        REQUIRE(pos2.y == 6.0f);
    }
}

TEST_CASE("Registry: 批量操作", "[ecs][registry]") {
    Registry registry;
    
    SECTION("创建100个实体") {
        std::vector<Entity> entities;
        for (int i = 0; i < 100; ++i) {
            auto entity = registry.create();
            registry.emplace<Position>(entity, static_cast<float>(i), static_cast<float>(i * 2));
            entities.push_back(entity);
        }
        
        REQUIRE(registry.size() == 100);
        
        for (int i = 0; i < 100; ++i) {
            REQUIRE(registry.valid(entities[i]));
            auto& pos = registry.get<Position>(entities[i]);
            REQUIRE(pos.x == static_cast<float>(i));
            REQUIRE(pos.y == static_cast<float>(i * 2));
        }
    }
    
    SECTION("清空所有实体") {
        for (int i = 0; i < 50; ++i) {
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.0f, 0.0f);
        }
        
        REQUIRE(registry.size() == 50);
        
        registry.clear();
        REQUIRE(registry.size() == 0);
    }
}

TEST_CASE("Registry: 边界情况", "[ecs][registry]") {
    Registry registry;
    
    SECTION("获取不存在的组件") {
        Entity entity = registry.create();
        REQUIRE(!registry.has<Position>(entity));
    }
    
    SECTION("销毁已销毁的实体") {
        Entity entity = registry.create();
        registry.destroy(entity);
        
        // 再次销毁不应该崩溃
        registry.destroy(entity);
    }
    
    SECTION("空 registry") {
        REQUIRE(registry.size() == 0);
    }
}

TEST_CASE("Registry: 性能测试", "[ecs][registry]") {
    Registry registry;
    
    SECTION("创建10000个实体") {
        const int count = 10000;
        for (int i = 0; i < count; ++i) {
            auto entity = registry.create();
            registry.emplace<Position>(entity, static_cast<float>(i), 0.0f);
            registry.emplace<Velocity>(entity, 0.0f, 1.0f);
        }
        
        REQUIRE(registry.size() == count);
    }
    
    SECTION("访问10000个实体的组件") {
        const int count = 10000;
        std::vector<Entity> entities;
        
        for (int i = 0; i < count; ++i) {
            auto entity = registry.create();
            registry.emplace<Position>(entity, static_cast<float>(i), 0.0f);
            entities.push_back(entity);
        }
        
        float sum = 0.0f;
        for (auto entity : entities) {
            auto& pos = registry.get<Position>(entity);
            sum += pos.x;
        }
        
        // 使用近似比较处理浮点数精度问题
        REQUIRE(Catch::Approx(sum).epsilon(0.01) == static_cast<float>(count * (count - 1) / 2));
    }
}
