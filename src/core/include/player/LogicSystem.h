//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDUCORE_LOGICSYSTEM_H
#define RENDUCORE_LOGICSYSTEM_H

#include "ecs/ISystem.h"

#include <cstdint>

// 游戏逻辑组件
struct PlayerComponent {
  uint32_t id;
  std::string name;
  uint32_t level;
  uint32_t experience;
};

struct MovementComponent {
  float target_x, target_y;
  float speed;
  bool moving = false;
};

struct HealthComponent {
  float current;
  float maximum;
};

// 游戏事件组件
struct PlayerLoginEvent {
  entt::entity player_entity;
};

struct PlayerMoveEvent {
  entt::entity player_entity;
  float from_x, from_y;
  float to_x, to_y;
};

class LogicSystem : public ISystem {
public:
  LogicSystem();

  void configure(entt::registry& registry) override;
  void update(entt::registry& registry, float delta_time) override;
  void initialize(entt::registry& registry) override;
  void shutdown(entt::registry& registry) override;
  std::string get_name() const override { return "LogicSystem"; }

private:
  void handle_network_events(entt::registry& registry);
  void update_movement(entt::registry& registry, float delta_time);
  void update_combat(entt::registry& registry, float delta_time);
  void update_ai(entt::registry& registry, float delta_time);

  void on_player_login(entt::registry& registry, const PlayerLoginEvent& event);
  void on_player_move(entt::registry& registry, const PlayerMoveEvent& event);
};

#endif // RENDUCORE_LOGICSYSTEM_H
