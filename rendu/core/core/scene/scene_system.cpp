/*
* Created by boil on 2023/3/19.
*/

#include "scene_system.h"

void rendu::scene_system::run(entt::registry &registry) {
  if (m_current_scene != nullptr) {
    m_current_scene->on_update(registry);
  }
}

void rendu::scene_system::change_scene(entt::registry &registry, std::unique_ptr<scene> new_scene) {
  if (m_current_scene != nullptr) {
    m_current_scene->on_exit(registry);
  }
  m_current_scene = std::move(new_scene);
  if (m_current_scene != nullptr) {
    m_current_scene->on_enter(registry);
  }
}
