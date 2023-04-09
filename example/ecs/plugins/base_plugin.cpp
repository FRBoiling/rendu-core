/*
* Created by boil on 2023/4/9.
*/

#include "base_plugin.h"
#include "base/game_loop.h"
#include "systems/event_system.h"
#include "systems/input_system.h"
#include "systems/input_system.h"
#include "systems/render_system.h"
#include "systems/messaging_system.h"
#include "components/transform_component.h"

void BasePlugin::mount(game_loop &game_loop) {
    game_loop
            .add_plugin(m_sdl)
            .add_plugin(m_imgui)
            .add_setup_callback([&](entt::registry &registry) {
                registry.ctx().emplace<std::shared_ptr<input::manager>>(m_input_manager);
//                registry.ctx().emplace<std::shared_ptr<messaging::bus>>(m_message_bus);
//                registry.ctx().emplace<std::shared_ptr<sys::debug_system>>(m_debug_system);

//                m_debug_system->register_component<transform_component>("Transform");
            })
            .add_frame_begin_callback([&](entt::registry &registry) {
                m_input_manager->frame_begin();
            })
//            .add_system(m_debug_system)
            .add_system(std::make_shared<EventSystem>())
            .add_system(std::make_shared<InputSystem>())
            .add_system_last(std::make_shared<MessagingSystem>())
            .add_system_last(std::make_shared<RenderSystem>());
}
