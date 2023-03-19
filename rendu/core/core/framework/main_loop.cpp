/*
* Created by boil on 2023/3/19.
*/

#include "main_loop.h"

void rendu::main_loop::run() {
  
  m_control_flow = ControlFlow::loop;
  m_registry.ctx().emplace<ControlFlow &>(m_control_flow);

  m_hook_setup.publish(m_registry);

  while (m_control_flow == ControlFlow::loop) {
    m_hook_frame_begin.publish(m_registry);
    for (auto &sys : m_systems) {
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
