/*
* Created by boil on 2023/4/9.
*/
#include "game_loop.h"

#include <ranges>
#include <utility>

game_loop &game_loop::add_plugin(Plugin &plugin) {
    plugin.mount(*this);
    return *this;
}

game_loop &game_loop::add_system(std::shared_ptr<System> system) {
    m_systems.push_back(system);
    return *this;
}

game_loop &game_loop::add_system_last(std::shared_ptr<System> system) {
    m_systems_last.push_back(system);
    return *this;
}

game_loop &game_loop::add_setup_callback(std::function<void(entt::registry &)> callback) {
    m_hook_setup.connect(std::move(callback));
    return *this;
}

game_loop &game_loop::add_frame_end_callback(std::function<void(entt::registry &)> callback) {
    m_hook_frame_end.connect(std::move(callback));
    return *this;
}

game_loop &game_loop::add_teardown_callback(std::function<void(entt::registry &)> callback) {
    m_hook_teardown.connect(std::move(callback), /* front = */ true);
    return *this;
}

game_loop &game_loop::add_frame_begin_callback(std::function<void(entt::registry &)> callback) {
    m_hook_frame_begin.connect(std::move(callback));
    return *this;
}

void game_loop::run() {
    m_control_flow = ControlFlow::loop;
    m_registry.ctx().emplace<ControlFlow &>(m_control_flow);

    m_hook_setup.publish(m_registry);

    while (m_control_flow == ControlFlow::loop) {
        m_hook_frame_begin.publish(m_registry);

        for (auto &sys: m_systems) {
            sys->run(m_registry);
        }

        // 反向迭代
        for (auto & sys : std::ranges::reverse_view(m_systems_last)) {
            sys->run(m_registry);
        }
    }

    m_hook_teardown.publish(m_registry);
}


