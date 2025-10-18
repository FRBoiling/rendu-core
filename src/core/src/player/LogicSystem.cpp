//
// Created by FRee2 on 2025/10/18.
//

#include "player/LogicSystem.h"


LogicSystem::LogicSystem() {
}

void LogicSystem::configure(entt::registry& registry) {
    // 注册事件处理
    registry.on_construct<PlayerLoginEvent>().connect<[this](entt::registry& reg, entt::entity entity) {
        if (reg.valid(entity) && reg.all_of<PlayerLoginEvent>(entity)) {
            on_player_login(reg, reg.get<PlayerLoginEvent>(entity));
            reg.destroy(entity); // 一次性事件，处理完就销毁
        }
    }>();

    registry.on_construct<PlayerMoveEvent>().connect<[this](entt::registry& reg, entt::entity entity) {
        if (reg.valid(entity) && reg.all_of<PlayerMoveEvent>(entity)) {
            on_player_move(reg, reg.get<PlayerMoveEvent>(entity));
            reg.destroy(entity); // 一次性事件，处理完就销毁
        }
    }>();
}

void LogicSystem::update(entt::registry& registry, float delta_time) {
    handle_network_events(registry);
    update_movement(registry, delta_time);
    update_combat(registry, delta_time);
    update_ai(registry, delta_time);
}

void LogicSystem::initialize(entt::registry& registry) {
    spdlog::info("Logic system initialized");
}

void LogicSystem::shutdown(entt::registry& registry) {
    spdlog::info("Logic system shutdown");
}

void LogicSystem::handle_network_events(entt::registry& registry) {
    // 处理网络消息
    if (auto* event = registry.ctx().find<MessageReceivedEvent>()) {
        // 处理网络消息
        if (registry.valid(event->message_entity) &&
            registry.all_of<NetworkMessageComponent>(event->message_entity)) {

            auto& message = registry.get<NetworkMessageComponent>(event->message_entity);

            // 解析协议，创建相应的游戏事件
            // 例如：玩家移动、攻击等

            spdlog::debug("Processing network message {}", message.id);
        }

        // 清理消息实体
        if (registry.valid(event->message_entity)) {
            registry.destroy(event->message_entity);
        }

        // 清理事件
        registry.ctx().erase<MessageReceivedEvent>();
    }
}

void LogicSystem::update_movement(entt::registry& registry, float delta_time) {
    auto view = registry.view<TransformComponent, MovementComponent>();

    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& movement = view.get<MovementComponent>(entity);

        if (movement.moving) {
            // 计算移动方向
            float dx = movement.target_x - transform.x;
            float dy = movement.target_y - transform.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 0.1f) {
                // 移动
                transform.x += (dx / distance) * movement.speed * delta_time;
                transform.y += (dy / distance) * movement.speed * delta_time;
            } else {
                // 到达目标
                movement.moving = false;
            }
        }
    }
}

void LogicSystem::update_combat(entt::registry& registry, float delta_time) {
    // 战斗逻辑更新
}

void LogicSystem::update_ai(entt::registry& registry, float delta_time) {
    // AI逻辑更新
}

void LogicSystem::on_player_login(entt::registry& registry, const PlayerLoginEvent& event) {
    spdlog::info("Player login: entity {}", static_cast<uint32_t>(event.player_entity));

    // 初始化玩家数据
    if (registry.valid(event.player_entity)) {
        // 发送初始游戏状态等
    }
}

void LogicSystem::on_player_move(entt::registry& registry, const PlayerMoveEvent& event) {
    if (registry.valid(event.player_entity) &&
        registry.all_of<TransformComponent, MovementComponent>(event.player_entity)) {

        auto& transform = registry.get<TransformComponent>(event.player_entity);
        auto& movement = registry.get<MovementComponent>(event.player_entity);

        movement.target_x = event.to_x;
        movement.target_y = event.to_y;
        movement.moving = true;

        spdlog::debug("Player {} moving to ({}, {})",
                     static_cast<uint32_t>(event.player_entity), event.to_x, event.to_y);
    }
}