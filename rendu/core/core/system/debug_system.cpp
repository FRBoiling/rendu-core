/*
* Created by boil on 2023/3/19.
*/

#include "debug_system.h"
#include <iostream>

void rendu::debug_system::run(entt::registry &registry) {
  std::cout << "run --=-----";
  // gather inputs
  if (m_open) {
    // 稍后再详细讲
//    auto &add_ui_command = registry.ctx().get<ui_command_request>();
//    add_ui_command([&](entt::registry& registry) {
//      ImGui::Begin("Inspector");
//
//      ImGui::Columns(2);
//      ImGui::BeginChild("Entity List");
//      m_editor.renderEntityList(registry, m_components_filter);
//      ImGui::EndChild();
//      ImGui::NextColumn();
//
//      ImGui::BeginChild("Entity Editor");
//      m_editor.renderEditor(registry, m_current_entity);
//      ImGui::EndChild();
//      ImGui::NextColumn();
//
//      ImGui::End();
//    });
  }
}
