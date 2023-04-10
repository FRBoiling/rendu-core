/*
* Created by boil on 2023/4/10.
*/

#ifndef RENDU_DEBUG_SYSTEM_H
#define RENDU_DEBUG_SYSTEM_H

#include "system.h"

class debug_system final : public System {
//private:
//    using Editor = MM::EntityEditor<entt::entity>;
//    Editor m_editor;
//
//    std::set<Editor::ComponentTypeID> m_components_filter;
//    entt::entity m_current_entity;
//
//    bool m_open{false};
//
//public:
//    template <typename Component>
//    void register_component(const std::string &display_name) {
//        m_editor.registerComponent<Component>(display_name);
//    }
//
//    virtual void run(entt::registry &registry) {
//        // gather inputs
//
//        if (/* backspace released */) {
//            m_open = !m_open;
//        }
//
//        if (m_open) {
//            // 稍后再详细讲
//            auto &add_ui_command = registry.ctx().get<ui_command_request>();
//            add_ui_command([&](entt::registry &registry) {
//                ImGui::Begin("Inspector");
//
//                ImGui::Columns(2);
//                ImGui::BeginChild("Entity List");
//                m_editor.renderEntityList(registry, m_components_filter);
//                ImGui::EndChild();
//                ImGui::NextColumn();
//
//                ImGui::BeginChild("Entity Editor");
//                m_editor.renderEditor(registry, m_current_entity);
//                ImGui::EndChild();
//                ImGui::NextColumn();
//
//                ImGui::End();
//            });
//        }
//    }
};


#endif //RENDU_DEBUG_SYSTEM_H
