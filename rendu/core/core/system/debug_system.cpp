/*
* Created by boil on 2023/3/19.
*/

#include "debug_system.h"
#include <iostream>

void rendu::debug_system::run(entt::registry &registry) {
  // gather inputs
  std::cout << "run -------" << std::endl;
  if (m_open) {
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
