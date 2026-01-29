#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "common/ecs/registry.h"
#include "common/ecs/view.h"
#include "common/ecs/system.h"

using namespace Rendu;

// ========== 组件定义 ==========

/// 位置组件
struct Position {
    float x;
    float y;

    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/// 速度组件
struct Velocity {
    float dx;
    float dy;

    Velocity(float dx = 0.0f, float dy = 0.0f) : dx(dx), dy(dy) {}
};

/// 名称组件
struct Name {
    std::string value;

    Name(const std::string& name) : value(name) {}
};

/// 生命值组件
struct Health {
    int32_t value;
    int32_t max_value;

    Health(int32_t value = 100, int32_t max_value = 100)
        : value(value), max_value(max_value) {}
};

/// 攻击力组件
struct Attack {
    int32_t value;

    explicit Attack(int32_t value = 10) : value(value) {}
};

// ========== 系统定义 ==========

/// 移动系统：更新实体位置
class MovementSystem : public ecs::System {
public:
    explicit MovementSystem(ecs::Registry& registry) : registry_(registry) {}

    void update(float delta_time) override {
        // 创建视图：只查询同时具有 Position 和 Velocity 组件的实体
        auto view = registry_.native().view<Position, Velocity>();

        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& vel = view.get<Velocity>(entity);

            pos.x += vel.dx * delta_time;
            pos.y += vel.dy * delta_time;
        }
    }

    std::string name() const override { return "MovementSystem"; }

private:
    ecs::Registry& registry_;
};

/// 生命值显示系统：输出实体生命值
class HealthDisplaySystem : public ecs::System {
public:
    explicit HealthDisplaySystem(ecs::Registry& registry) : registry_(registry) {}

    void update(float delta_time) override {
        // 创建视图：查询具有 Name 和 Health 组件的实体
        auto view = registry_.native().view<Name, Health>();

        for (auto entity : view) {
            auto& name = view.get<Name>(entity);
            auto& health = view.get<Health>(entity);

            std::cout << "[健康] " << name.value
                      << " - HP: " << health.value << "/" << health.max_value;

            if (health.value <= 0) {
                std::cout << " (已死亡)";
            } else if (health.value < health.max_value * 0.3f) {
                std::cout << " (危险)";
            }

            std::cout << std::endl;
        }
    }

    std::string name() const override { return "HealthDisplaySystem"; }

private:
    ecs::Registry& registry_;
};

/// 战斗系统：处理攻击逻辑
class CombatSystem : public ecs::System {
public:
    explicit CombatSystem(ecs::Registry& registry) : registry_(registry) {}

    void update(float delta_time) override {
        // 查询所有具有 Attack 和 Health 组件的实体
        auto attackers = registry_.native().view<Name, Attack>();

        for (auto attacker : attackers) {
            auto& attacker_name = attackers.get<Name>(attacker);
            auto& attack = attackers.get<Attack>(attacker);

            // 查找其他实体进行攻击
            auto targets = registry_.native().view<Name, Health>();

            for (auto target : targets) {
                // 不攻击自己
                if (attacker == target) continue;

                // 随机攻击（简化逻辑）
                if (rand() % 100 < 20) {  // 20% 概率攻击
                    auto& target_name = targets.get<Name>(target);
                    auto& target_health = targets.get<Health>(target);

                    if (target_health.value > 0) {
                        target_health.value -= attack.value;
                        std::cout << "[战斗] " << attacker_name.value
                                  << " 攻击了 " << target_name.value
                                  << ", 造成 " << attack.value << " 点伤害" << std::endl;

                        if (target_health.value < 0) {
                            target_health.value = 0;
                        }
                    }
                }
            }
        }
    }

    std::string name() const override { return "CombatSystem"; }

private:
    ecs::Registry& registry_;
};

/// 统计系统：统计实体数量
class StatisticsSystem : public ecs::System {
public:
    explicit StatisticsSystem(ecs::Registry& registry) : registry_(registry) {}

    int32_t total_entities() const { return total_entities_; }
    int32_t alive_entities() const { return alive_entities_; }
    int32_t dead_entities() const { return dead_entities_; }

    void update(float delta_time) override {
        // 统计总实体数
        auto all_view = registry_.native().view<entt::entity>();
        total_entities_ = static_cast<int32_t>(all_view.size());

        // 统计存活和死亡的实体
        auto health_view = registry_.native().view<Health>();
        alive_entities_ = 0;
        dead_entities_ = 0;

        for (auto entity : health_view) {
            auto& health = health_view.get<Health>(entity);
            if (health.value > 0) {
                alive_entities_++;
            } else {
                dead_entities_++;
            }
        }
    }

    std::string name() const override { return "StatisticsSystem"; }

private:
    ecs::Registry& registry_;
    int32_t total_entities_{0};
    int32_t alive_entities_{0};
    int32_t dead_entities_{0};
};

int main() {
    std::cout << "========== ECS 示例程序 ==========" << std::endl;
    std::cout << std::endl;

    // 创建实体注册表
    ecs::Registry registry;
    std::cout << "[ECS] 实体注册表已创建" << std::endl;

    // 创建系统（注意：由于 SystemManager 不支持传递 Registry，这里不使用它）
    auto movement_system = std::make_shared<MovementSystem>(registry);
    auto health_display_system = std::make_shared<HealthDisplaySystem>(registry);
    auto combat_system = std::make_shared<CombatSystem>(registry);
    auto stats_system = std::make_shared<StatisticsSystem>(registry);

    std::cout << "[ECS] 已创建 4 个系统" << std::endl;
    std::cout << std::endl;

    std::cout << "=== 创建实体 ===" << std::endl;

    // 创建玩家实体
    auto player = registry.create();
    registry.emplace<Name>(player, "玩家");
    registry.emplace<Position>(player, 0.0f, 0.0f);
    registry.emplace<Velocity>(player, 1.0f, 0.5f);
    registry.emplace<Health>(player, 100, 100);
    registry.emplace<Attack>(player, 15);
    std::cout << "[ECS] 已创建实体: 玩家 (ID: " << static_cast<uint32_t>(player) << ")" << std::endl;

    // 创建敌人实体
    auto enemy1 = registry.create();
    registry.emplace<Name>(enemy1, "敌人1");
    registry.emplace<Position>(enemy1, 10.0f, 5.0f);
    registry.emplace<Velocity>(enemy1, -0.5f, 0.3f);
    registry.emplace<Health>(enemy1, 80, 80);
    registry.emplace<Attack>(enemy1, 10);

    auto enemy2 = registry.create();
    registry.emplace<Name>(enemy2, "敌人2");
    registry.emplace<Position>(enemy2, -5.0f, 8.0f);
    registry.emplace<Velocity>(enemy2, 0.3f, -0.2f);
    registry.emplace<Health>(enemy2, 60, 60);
    registry.emplace<Attack>(enemy2, 8);

    auto enemy3 = registry.create();
    registry.emplace<Name>(enemy3, "敌人3");
    registry.emplace<Position>(enemy3, 3.0f, -4.0f);
    registry.emplace<Velocity>(enemy3, -0.2f, 0.4f);
    registry.emplace<Health>(enemy3, 50, 50);
    registry.emplace<Attack>(enemy3, 5);

    std::cout << "[ECS] 已创建 3 个敌人实体" << std::endl;

    // 创建 NPC 实体（无战斗能力）
    auto npc = registry.create();
    registry.emplace<Name>(npc, "村民");
    registry.emplace<Position>(npc, 2.0f, 2.0f);
    registry.emplace<Velocity>(npc, 0.1f, 0.1f);
    registry.emplace<Health>(npc, 30, 30);
    std::cout << "[ECS] 已创建实体: 村民 (ID: " << static_cast<uint32_t>(npc) << ")" << std::endl;

    std::cout << std::endl;
    std::cout << "=== 模拟游戏循环 (10 帧) ===" << std::endl;
    std::cout << std::endl;

    const float delta_time = 1.0f;
    const int32_t frames = 10;

    for (int32_t frame = 1; frame <= frames; ++frame) {
        std::cout << "--- 帧 " << frame << " ---" << std::endl;

        // 执行移动系统
        movement_system->update(delta_time);

        // 执行战斗系统
        combat_system->update(delta_time);

        // 显示生命值
        health_display_system->update(delta_time);

        // 统计信息
        stats_system->update(delta_time);
        std::cout << "[统计] 总实体: " << stats_system->total_entities()
                  << ", 存活: " << stats_system->alive_entities()
                  << ", 死亡: " << stats_system->dead_entities() << std::endl;

        std::cout << std::endl;

        // 如果所有敌人都死亡，提前结束
        if (stats_system->alive_entities() <= 1) {
            std::cout << "[游戏] 所有敌人已被击败！" << std::endl;
            break;
        }
    }

    std::cout << "=== 组件查询演示 ===" << std::endl;

    // 查询所有具有 Name 和 Position 的实体
    auto pos_view = registry.native().view<Name, Position>();
    std::cout << "[查询] 具有位置的实体:" << std::endl;
    for (auto entity : pos_view) {
        auto& name = pos_view.get<Name>(entity);
        auto& pos = pos_view.get<Position>(entity);
        std::cout << "  - " << name.value << ": (" << pos.x << ", " << pos.y << ")" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== 组件修改演示 ===" << std::endl;

    // 修改玩家位置
    if (registry.has<Position>(player)) {
        auto& pos = registry.get<Position>(player);
        std::cout << "[修改] 玩家位置: (" << pos.x << ", " << pos.y << ")";
        pos.x = 100.0f;
        pos.y = 100.0f;
        std::cout << " -> (" << pos.x << ", " << pos.y << ")" << std::endl;
    }

    // 修改玩家生命值
    if (registry.has<Health>(player)) {
        auto& health = registry.get<Health>(player);
        std::cout << "[修改] 玩家生命值: " << health.value;
        health.value = health.max_value;
        std::cout << " -> " << health.value << " (已完全恢复)" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== 组件移除演示 ===" << std::endl;

    // 移除村民的速度组件（使其无法移动）
    if (registry.has<Velocity>(npc)) {
        registry.remove<Velocity>(npc);
        std::cout << "[移除] 村民已失去移动能力" << std::endl;
    }

    // 检查村民是否还有速度组件
    std::cout << "[检查] 村民是否具有速度组件: "
              << (registry.has<Velocity>(npc) ? "是" : "否") << std::endl;

    std::cout << std::endl;
    std::cout << "=== 最终统计 ===" << std::endl;
    std::cout << "总实体数量: " << registry.size() << std::endl;

    std::cout << std::endl;
    std::cout << "========== 示例程序结束 ==========" << std::endl;

    return 0;
}
