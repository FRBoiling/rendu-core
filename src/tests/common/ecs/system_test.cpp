//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <common/ecs/system.h>
#include <common/ecs/registry.h>
#include <memory>

using namespace Rendu::ecs;

struct Position {
    float x, y;
};

struct Velocity {
    float vx, vy;
};

class MovementSystem : public System {
public:
    MovementSystem(Registry& registry) : registry_(registry), update_count_(0) {}

    void update(float delta_time) override {
        update_count_++;
        
        auto view = registry_.native().view<Position, Velocity>();
        view.each([delta_time](auto& pos, auto& vel) {
            pos.x += vel.vx * delta_time;
            pos.y += vel.vy * delta_time;
        });
    }

    std::string name() const override { return "MovementSystem"; }
    int priority() const override { return 0; }
    
    int update_count() const { return update_count_; }

private:
    Registry& registry_;
    int update_count_;
};

class PhysicsSystem : public System {
public:
    PhysicsSystem(Registry& registry) : registry_(registry), update_count_(0) {}

    void update(float delta_time) override {
        update_count_++;
        // 物理模拟逻辑
    }

    std::string name() const override { return "PhysicsSystem"; }
    int priority() const override { return 1; } // 在 MovementSystem 之后执行
    
    int update_count() const { return update_count_; }

private:
    Registry& registry_;
    int update_count_;
};

class RenderSystem : public System {
public:
    RenderSystem(Registry& registry) : registry_(registry), update_count_(0) {}

    void update(float delta_time) override {
        update_count_++;
        // 渲染逻辑
    }

    std::string name() const override { return "RenderSystem"; }
    int priority() const override { return 10; } // 最后执行
    
    int update_count() const { return update_count_; }

private:
    Registry& registry_;
    int update_count_;
};

TEST_CASE("SystemManager: 添加系统", "[ecs][system]") {
    SystemManager manager;
    Registry registry;
    
    SECTION("添加单个系统") {
        auto system = std::make_shared<MovementSystem>(registry);
        manager.add_system(system);
        
        REQUIRE(manager.system_count() == 1);
    }
    
    SECTION("添加多个系统") {
        manager.add_system(std::make_shared<MovementSystem>(registry));
        manager.add_system(std::make_shared<PhysicsSystem>(registry));
        manager.add_system(std::make_shared<RenderSystem>(registry));
        
        REQUIRE(manager.system_count() == 3);
    }
    
    SECTION("添加空指针") {
        manager.add_system(nullptr);
        REQUIRE(manager.system_count() == 0);
    }
}

TEST_CASE("SystemManager: 系统执行顺序", "[ecs][system]") {
    SystemManager manager;
    Registry registry;
    
    manager.add_system(std::make_shared<RenderSystem>(registry));
    manager.add_system(std::make_shared<MovementSystem>(registry));
    manager.add_system(std::make_shared<PhysicsSystem>(registry));
    
    const auto& systems = manager.systems();
    
    // 按优先级排序：MovementSystem(0), PhysicsSystem(1), RenderSystem(10)
    REQUIRE(systems[0]->priority() == 0);
    REQUIRE(systems[0]->name() == "MovementSystem");
    
    REQUIRE(systems[1]->priority() == 1);
    REQUIRE(systems[1]->name() == "PhysicsSystem");
    
    REQUIRE(systems[2]->priority() == 10);
    REQUIRE(systems[2]->name() == "RenderSystem");
}

TEST_CASE("SystemManager: 移除系统", "[ecs][system]") {
    SystemManager manager;
    Registry registry;
    
    manager.add_system(std::make_shared<MovementSystem>(registry));
    manager.add_system(std::make_shared<PhysicsSystem>(registry));
    
    REQUIRE(manager.system_count() == 2);
    
    manager.remove_system("MovementSystem");
    REQUIRE(manager.system_count() == 1);
    
    const auto& systems = manager.systems();
    REQUIRE(systems[0]->name() == "PhysicsSystem");
}

TEST_CASE("SystemManager: 更新系统", "[ecs][system]") {
    SystemManager manager;
    Registry registry;
    
    auto movement = std::make_shared<MovementSystem>(registry);
    auto physics = std::make_shared<PhysicsSystem>(registry);
    
    manager.add_system(movement);
    manager.add_system(physics);
    
    SECTION("单次更新") {
        manager.update(0.016f);
        
        REQUIRE(movement->update_count() == 1);
        REQUIRE(physics->update_count() == 1);
    }
    
    SECTION("多次更新") {
        manager.update(0.016f);
        manager.update(0.016f);
        manager.update(0.016f);
        
        REQUIRE(movement->update_count() == 3);
        REQUIRE(physics->update_count() == 3);
    }
}

TEST_CASE("SystemManager: 系统启用/禁用", "[ecs][system]") {
    SystemManager manager;
    Registry registry;
    
    auto movement = std::make_shared<MovementSystem>(registry);
    auto physics = std::make_shared<PhysicsSystem>(registry);
    
    manager.add_system(movement);
    manager.add_system(physics);
    
    SECTION("禁用系统") {
        physics->set_enabled(false);
        
        manager.update(0.016f);
        
        REQUIRE(movement->update_count() == 1);
        REQUIRE(physics->update_count() == 0);
    }
    
    SECTION("重新启用系统") {
        physics->set_enabled(false);
        manager.update(0.016f);
        
        REQUIRE(physics->update_count() == 0);
        
        physics->set_enabled(true);
        manager.update(0.016f);
        
        REQUIRE(physics->update_count() == 1);
    }
}

TEST_CASE("SystemManager: 集成测试", "[ecs][system]") {
    SystemManager manager;
    Registry registry;
    
    // 创建实体
    Entity entity = registry.create();
    registry.emplace<Position>(entity, 0.0f, 0.0f);
    registry.emplace<Velocity>(entity, 1.0f, 2.0f);
    
    manager.add_system(std::make_shared<MovementSystem>(registry));
    
    SECTION("系统更新实体组件") {
        manager.update(1.0f);
        
        auto& pos = registry.get<Position>(entity);
        REQUIRE(pos.x == 1.0f);
        REQUIRE(pos.y == 2.0f);
        
        manager.update(1.0f);
        
        REQUIRE(pos.x == 2.0f);
        REQUIRE(pos.y == 4.0f);
    }
}

TEST_CASE("System: 基本接口", "[ecs][system]") {
    Registry registry;
    MovementSystem system(registry);
    
    SECTION("默认启用") {
        REQUIRE(system.is_enabled() == true);
    }
    
    SECTION("名称") {
        REQUIRE(system.name() == "MovementSystem");
    }
    
    SECTION("优先级") {
        REQUIRE(system.priority() == 0);
    }
}
