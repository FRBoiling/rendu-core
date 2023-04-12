/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_BASE_HOST_H_
#define RENDU_CORE_BASE_HOST_H_

#include <entt/entt.hpp>
#include "hook.h"
#include "system.h"
#include "plugin.h"

namespace rendu {

    enum class ControlFlow {
        exit,
        loop
    };

    class host {
    private:
        entt::registry m_registry;
        ControlFlow m_control_flow;
        std::vector<std::shared_ptr<system>> m_systems;
        std::vector<std::shared_ptr<system>> m_systems_last; // very dumb way of specifying when a system should execute
        hook<void(entt::registry &)> m_hook_setup;
        hook<void(entt::registry &)> m_hook_frame_begin;
        hook<void(entt::registry &)> m_hook_frame_end;
        hook<void(entt::registry &)> m_hook_teardown;
    public:
        host &add_system(std::shared_ptr<system> system) {
            m_systems.push_back(system);
            return *this;
        }

        host &add_system_last(std::shared_ptr<system> system) {
            m_systems_last.push_back(system);
            return *this;
        }

        host &add_setup_callback(std::function<void(entt::registry &)> callback) {
            m_hook_setup.connect(callback);
            return *this;
        }

        host &add_frame_begin_callback(std::function<void(entt::registry &)> callback) {
            m_hook_frame_begin.connect(callback);
            return *this;
        }

        host &add_frame_end_callback(std::function<void(entt::registry &)> callback) {
            m_hook_frame_end.connect(callback);
            return *this;
        }

        host &add_teardown_callback(std::function<void(entt::registry &)> callback) {
            m_hook_teardown.connect(callback, /* front = */ true);
            return *this;
        }

        host &add_plugin(plugin &plugin) {
            plugin.mount(*this);
            return *this;
        }

        void run() {

            m_control_flow = ControlFlow::loop;
            m_registry.ctx().emplace<ControlFlow &>(m_control_flow);

            m_hook_setup.publish(m_registry);

            while (m_control_flow == ControlFlow::loop) {
                m_hook_frame_begin.publish(m_registry);
                for (auto &sys: m_systems) {
                    sys->run(m_registry);
                }
                // 反向迭代
                for (auto it = m_systems_last.rbegin(); it != m_systems_last.rend(); ++it) {
                    auto &sys = *it;
                    sys->run(m_registry);
                }
            }

            m_hook_teardown.publish(m_registry);

        }
    };

}

#endif //RENDU_CORE_BASE_HOST_H_
