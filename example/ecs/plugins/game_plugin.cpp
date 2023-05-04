/*
* Created by boil on 2023/4/9.
*/

#include "game_plugin.h"
#include "base/game_loop.h"
#include "scene_menu_main.h"

void game_plugin::mount(game_loop &game_loop) {
    game_loop
            .add_setup_callback([&](entt::registry &registry) {
                registry.ctx().emplace<std::shared_ptr<SceneSystem>>(m_scene_manager);
                m_scene_manager->change_scene(registry, std::make_unique<scene_menu_main>());
            })
            .add_teardown_callback([&](entt::registry &registry) {
                m_scene_manager->change_scene(registry, nullptr);
            })
            .add_system(m_scene_manager);
}
