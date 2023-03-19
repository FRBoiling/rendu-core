/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_SCENE_SYSTEM_H_
#define RENDU_CORE_CORE_SCENE_SYSTEM_H_

#include "framework/system.h"
#include <scene/scene.h>
#include <iostream>

namespace rendu {

class scene_system final : public system {
 private:
  std::unique_ptr<scene> m_current_scene{nullptr};

 public:
  void run(entt::registry &registry) override;
  void change_scene(entt::registry &registry, std::unique_ptr<scene> new_scene);
};

}

#endif //RENDU_CORE_CORE_SCENE_SYSTEM_H_
