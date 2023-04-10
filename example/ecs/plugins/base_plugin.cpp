/*
* Created by boil on 2023/4/9.
*/

#include "base_plugin.h"
#include "base/game_loop.h"
#include "systems/event_system.h"
#include "systems/input_system.h"

void BasePlugin::mount(game_loop &game_loop) {
    game_loop
            .add_plugin(m_sdl)
            .add_setup_callback([&](entt::registry &registry) {
                 registry.ctx().emplace<std::shared_ptr<input::manager>>(m_input_manager);
            })
            .add_frame_begin_callback([&](entt::registry &registry) {
                m_input_manager->frame_begin();
            })
            .add_system(std::make_shared<EventSystem>())
            .add_system(std::make_shared<InputSystem>())
            ;
}
