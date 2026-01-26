//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <common/ecs/view.h>
#include <common/ecs/registry.h>

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

TEST_CASE("View: 基本查询", "[ecs][view]") {
    Registry registry;
    
    SECTION("空视图") {
        View<Position> view(registry.native());
        REQUIRE(view.size() == 0);
        REQUIRE(view.empty() == true);
    }
    
    SECTION("单个组件视图") {
        Entity e1 = registry.create();
        registry.emplace<Position>(e1, 1.0f, 2.0f);
        
        View<Position> view(registry.native());
        REQUIRE(view.size() == 1);
        REQUIRE(view.empty() == false);
    }
    
    SECTION("多个组件视图") {
        Entity e1 = registry.create();
        registry.emplace<Position>(e1, 1.0f, 2.0f);
        registry.emplace<Velocity>(e1, 3.0f, 4.0f);
        
        View<Position, Velocity> view(registry.native());
        REQUIRE(view.size() == 1);
    }
}

TEST_CASE("View: 遍历实体", "[ecs][view]") {
    Registry registry;
    
    SECTION("遍历单个组件") {
        Entity e1 = registry.create();
        Entity e2 = registry.create();
        Entity e3 = registry.create();
        
        registry.emplace<Position>(e1, 1.0f, 0.0f);
        registry.emplace<Position>(e2, 2.0f, 0.0f);
        registry.emplace<Position>(e3, 3.0f, 0.0f);
        
        View<Position> view(registry.native());
        
        int count = 0;
        float sum = 0.0f;
        view.for_each([&](auto& pos) {
            count++;
            sum += pos.x;
        });
        
        REQUIRE(count == 3);
        REQUIRE(sum == 6.0f);
    }
    
    SECTION("遍历多个组件") {
        Entity e1 = registry.create();
        Entity e2 = registry.create();
        
        registry.emplace<Position>(e1, 1.0f, 2.0f);
        registry.emplace<Velocity>(e1, 3.0f, 4.0f);
        
        registry.emplace<Position>(e2, 5.0f, 6.0f);
        registry.emplace<Velocity>(e2, 7.0f, 8.0f);
        
        View<Position, Velocity> view(registry.native());
        
        int count = 0;
        view.for_each([&](auto& pos, auto& vel) {
            count++;
        });
        
        REQUIRE(count == 2);
    }
}

TEST_CASE("View: 过滤实体", "[ecs][view]") {
    Registry registry;
    
    SECTION("只查询有特定组件的实体") {
        Entity e1 = registry.create();
        Entity e2 = registry.create();
        Entity e3 = registry.create();
        
        // e1 有 Position 和 Velocity
        registry.emplace<Position>(e1, 1.0f, 2.0f);
        registry.emplace<Velocity>(e1, 3.0f, 4.0f);
        
        // e2 只有 Position
        registry.emplace<Position>(e2, 5.0f, 6.0f);
        
        // e3 只有 Velocity
        registry.emplace<Velocity>(e3, 7.0f, 8.0f);
        
        View<Position, Velocity> view(registry.native());
        REQUIRE(view.size() == 1); // 只有 e1
    }
    
    SECTION("查询特定实体") {
        Entity e1 = registry.create();
        Entity e2 = registry.create();
        
        registry.emplace<Position>(e1, 1.0f, 2.0f);
        registry.emplace<Position>(e2, 5.0f, 6.0f);
        
        View<Position> view(registry.native());
        
        REQUIRE(view.contains(e1) == true);
        REQUIRE(view.contains(e2) == true);
        REQUIRE(view.contains(entt::null) == false);
    }
}

TEST_CASE("View: 获取组件", "[ecs][view]") {
    Registry registry;
    
    Entity e1 = registry.create();
    registry.emplace<Position>(e1, 1.0f, 2.0f);
    registry.emplace<Velocity>(e1, 3.0f, 4.0f);
    
    View<Position, Velocity> view(registry.native());
    
    SECTION("获取单个组件") {
        auto& pos = view.get<Position>(e1);
        REQUIRE(pos.x == 1.0f);
        REQUIRE(pos.y == 2.0f);
        
        auto& vel = view.get<Velocity>(e1);
        REQUIRE(vel.vx == 3.0f);
        REQUIRE(vel.vy == 4.0f);
    }
}

TEST_CASE("View: 复杂场景", "[ecs][view]") {
    Registry registry;
    
    SECTION("游戏场景：移动系统") {
        // 创建玩家实体
        Entity player = registry.create();
        registry.emplace<Position>(player, 0.0f, 0.0f);
        registry.emplace<Velocity>(player, 1.0f, 1.0f);
        registry.emplace<Health>(player, 100);
        
        // 创建敌人实体
        Entity enemy = registry.create();
        registry.emplace<Position>(enemy, 10.0f, 10.0f);
        registry.emplace<Velocity>(enemy, -1.0f, 0.0f);
        registry.emplace<Health>(enemy, 50);
        
        // 创建障碍物（没有速度）
        Entity obstacle = registry.create();
        registry.emplace<Position>(obstacle, 5.0f, 5.0f);
        registry.emplace<Health>(obstacle, 200);
        
        // 查询可移动的实体（有 Position 和 Velocity）
        View<Position, Velocity> view(registry.native());
        
        REQUIRE(view.size() == 2); // player 和 enemy
        
        float delta_time = 1.0f;
        view.for_each([&](auto& pos, auto& vel) {
            pos.x += vel.vx * delta_time;
            pos.y += vel.vy * delta_time;
        });
        
        auto& player_pos = registry.get<Position>(player);
        auto& enemy_pos = registry.get<Position>(enemy);
        auto& obstacle_pos = registry.get<Position>(obstacle);
        
        REQUIRE(player_pos.x == 1.0f);
        REQUIRE(enemy_pos.x == 9.0f);
        REQUIRE(obstacle_pos.x == 5.0f); // 障碍物没有移动
    }
    
    SECTION("批量创建和查询") {
        const int count = 100;
        
        for (int i = 0; i < count; ++i) {
            Entity e = registry.create();
            registry.emplace<Position>(e, static_cast<float>(i), static_cast<float>(i * 2));
            
            if (i % 2 == 0) {
                registry.emplace<Velocity>(e, 1.0f, 1.0f);
            }
        }
        
        View<Position, Velocity> view(registry.native());
        REQUIRE(view.size() == count / 2);
        
        int sum = 0;
        view.for_each([&](auto& pos, auto& vel) {
            sum += static_cast<int>(pos.x);
        });
        
        REQUIRE(sum == (count / 2) * ((count / 2) - 1));
    }
}

TEST_CASE("View: 性能测试", "[ecs][view]") {
    Registry registry;
    
    SECTION("创建10000个实体并查询") {
        const int count = 10000;
        
        for (int i = 0; i < count; ++i) {
            Entity e = registry.create();
            registry.emplace<Position>(e, static_cast<float>(i), 0.0f);
            
            if (i % 2 == 0) {
                registry.emplace<Velocity>(e, 1.0f, 0.0f);
            }
        }
        
        View<Position> all_view(registry.native());
        REQUIRE(all_view.size() == count);
        
        View<Position, Velocity> moving_view(registry.native());
        REQUIRE(moving_view.size() == count / 2);
        
        float sum = 0.0f;
        all_view.for_each([&](auto& pos) {
            sum += pos.x;
        });
        
        // 使用近似比较处理浮点数精度问题
        REQUIRE(Catch::Approx(sum).epsilon(0.01) == static_cast<float>(count * (count - 1) / 2));
    }
}

TEST_CASE("View: 边界情况", "[ecs][view]") {
    Registry registry;
    
    SECTION("空实体引用") {
        View<Position> view(registry.native());
        REQUIRE(view.contains(entt::null) == false);
    }
    
    SECTION("销毁实体后的视图") {
        Entity e1 = registry.create();
        Entity e2 = registry.create();
        
        registry.emplace<Position>(e1, 1.0f, 2.0f);
        registry.emplace<Position>(e2, 3.0f, 4.0f);
        
        View<Position> view(registry.native());
        REQUIRE(view.size() == 2);
        
        registry.destroy(e1);
        REQUIRE(view.size() == 1);
    }
}
