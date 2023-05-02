/*
* Created by boil on 2023/3/19.
*/

#include "cmd_io_plugin.h"
#include "base/Host.h"
#include "cmd_io_system.h"

using namespace rendu;

CmdIoPlugin::CmdIoPlugin() = default;

void CmdIoPlugin::Mount(Host &host) {
   host.AddSystem(std::make_shared<CmdIoSystem>());
//        .add_setup_handler([&](entt::registry &registry) {
//        registry.ctx().emplace<std::shared_ptr<input::manager>>(m_input_manager);
//        registry.ctx().emplace<std::shared_ptr<messaging::bus>>(m_message_bus);
//        registry.ctx().emplace<std::shared_ptr<sys::debug_system>>(m_debug_system);
//
//        m_debug_system->register_component<components::transform>("Transform");
//        m_debug_system->register_component<components::camera>("Camera");
//        m_debug_system->register_component<components::background>("Background");
//        m_debug_system->register_component<components::sprite>("Sprite");
//      })
//      .add_frame_begin_handler([&](entt::registry &registry) {
//        m_input_manager->frame_begin();
//      })
//      .add_system(m_debug_system)
//      .add_system(std::make_shared<EventSystem>())
//      .add_system(std::make_shared<input_system>())
//      .add_system_last(std::make_shared<messaging_system>())
//      .add_system_last(std::make_shared<RenderSystem>());
}
