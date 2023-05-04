/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_GAME_LOOP_H
#define RENDU_GAME_LOOP_H

#include "hook.h"

#include "base/system.h"
#include "base/plugin.h"

class game_loop {
private:
    entt::registry m_registry;
    ControlFlow m_control_flow;
    std::vector<std::shared_ptr<System>> m_systems;
    std::vector<std::shared_ptr<System>> m_systems_last; // very dumb way of specifying when a system should execute

    hook<void(entt::registry &)> m_hook_setup;
    hook<void(entt::registry &)> m_hook_frame_begin;
    hook<void(entt::registry &)> m_hook_frame_end;
    hook<void(entt::registry &)> m_hook_teardown;

public:
    game_loop &add_plugin(Plugin &plugin);

    game_loop &add_system(std::shared_ptr<System> system);

    game_loop &add_system_last(std::shared_ptr<System> system);

    game_loop &add_setup_callback(std::function<void(entt::registry &)> callback);

    game_loop &add_frame_begin_callback(std::function<void(entt::registry &)> callback);

    game_loop &add_frame_end_callback(std::function<void(entt::registry &)> callback);

    game_loop &add_teardown_callback(std::function<void(entt::registry &)> callback);

    void run();
};

#endif //RENDU_GAME_LOOP_H
