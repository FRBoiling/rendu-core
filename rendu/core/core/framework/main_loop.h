/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_FRAMEWORK_MAIN_LOOP_H_
#define RENDU_CORE_CORE_FRAMEWORK_MAIN_LOOP_H_

#include <entt/entt.hpp>
#include "system.h"
#include "hook.h"
#include "plugin.h"

namespace rendu {

enum class ControlFlow {
  exit,
  loop
};

class plugin;

class main_loop {
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
  main_loop &add_system(std::shared_ptr<system> system) {
    m_systems.push_back(system);
    return *this;
  }
  main_loop &add_system_last(std::shared_ptr<system> system) {
    m_systems_last.push_back(system);
    return *this;
  }
  main_loop &add_setup_callback(std::function<void(entt::registry &)> callback) {
    m_hook_setup.connect(callback);
    return *this;
  }
  main_loop &add_frame_begin_callback(std::function<void(entt::registry &)> callback) {
    m_hook_frame_begin.connect(callback);
    return *this;
  }
  main_loop &add_frame_end_callback(std::function<void(entt::registry &)> callback) {
    m_hook_frame_end.connect(callback);
    return *this;
  }
  main_loop &add_teardown_callback(std::function<void(entt::registry &)> callback) {
    m_hook_teardown.connect(callback, /* front = */ true);
    return *this;
  }
  main_loop &add_plugin(plugin &plugin) {
    plugin.mount(*this);
    return *this;
  }
  void run();
};

}

#endif //RENDU_CORE_CORE_FRAMEWORK_MAIN_LOOP_H_
