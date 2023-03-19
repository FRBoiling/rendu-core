/*
* Created by boil on 2023/3/19.
*/

#include "main_scene.h"

void rendu::main_scene::on_enter(entt::registry &registry) {
    auto entity = registry.create();
    // 为entity添加组件
    m_entities.push_back(entity);
}

void rendu::main_scene::on_update(entt::registry &registry) {
//    auto &add_ui_command = registry.ctx().get<imgui_plugin::ui_command_request>();
//    add_ui_command([&](entt::registry &registry) {
//      // 呈现ImGui界面
//    });
}

void rendu::main_scene::on_exit(entt::registry &registry) {
  registry.destroy(m_entities.begin(), m_entities.end());
  m_entities.clear();
}