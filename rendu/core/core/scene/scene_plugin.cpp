/*
* Created by boil on 2023/3/19.
*/

#include "scene_plugin.h"
#include "base/host.h"

void rendu::scene_plugin::mount(rendu::host &main_loop) {
  main_loop
      .add_setup_callback([&](entt::registry &registry) {
        registry.ctx().emplace<std::shared_ptr<scene_system>>(m_scene_manager);
      })
      .add_teardown_callback([&](entt::registry &registry) {
        m_scene_manager->change_scene(registry, nullptr);
      })
      .add_system(m_scene_manager);
}
