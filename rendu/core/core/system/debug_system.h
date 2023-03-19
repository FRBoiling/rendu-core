/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_SYSTEM_DEBUG_SYSTEM_H_
#define RENDU_CORE_CORE_SYSTEM_DEBUG_SYSTEM_H_

#include "framework/system.h"

namespace rendu {

class debug_system final : public system {

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

#endif //RENDU_CORE_CORE_SYSTEM_DEBUG_SYSTEM_H_
