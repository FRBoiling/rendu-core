/*
* Created by boil on 2023/3/19.
*/

#include "base_plugin.h"
#include <entity/registry.hpp>
#include "framework/main_loop.h"

void rendu::base_plugin::mount(rendu::main_loop &main_loop) {
  main_loop
      .add_plugin(m_cmd_line)
      .add_setup_callback([&](entt::registry &registry) {
        registry.ctx().emplace<std::shared_ptr<base_system>>(m_base_system);
//        registry.ctx().emplace<std::shared_ptr<input::manager>>(m_input_manager);
//        registry.ctx().emplace<std::shared_ptr<messaging::bus>>(m_message_bus);
//
//        m_debug_system->register_component<components::transform>("Transform");
//        m_debug_system->register_component<components::background>("Background");
//        m_debug_system->register_component<components::sprite>("Sprite");
      })
      .add_frame_begin_callback([&](entt::registry &registry) {
//        m_input_manager->frame_begin();
      })
      .add_system(m_base_system);
//      .add_system(std::make_shared<event_system>())
//      .add_system(std::make_shared<input_system>())
//      .add_system_last(std::make_shared<messaging_system>())
//      .add_system_last(std::make_shared<render_system>());
}
