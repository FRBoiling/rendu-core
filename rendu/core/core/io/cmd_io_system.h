/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_IO_CMD_IO_SYSTEM_H_
#define RENDU_CORE_IO_CMD_IO_SYSTEM_H_

#include "base/system.h"

namespace rendu {

class cmd_io_system final : public system {

 private:
  entt::entity m_current_entity;
  bool m_open{false};
 public:
  template<typename Component>
  void register_component(const std::string &display_name) {
//    m_editor.registerComponent<Component>(display_name);
  }
  void run(entt::registry &registry) override;
};

}

#endif //RENDU_CORE_IO_CMD_IO_SYSTEM_H_
