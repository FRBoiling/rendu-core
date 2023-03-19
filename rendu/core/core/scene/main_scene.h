/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_SCENE_MAIN_SCENE_H_
#define RENDU_CORE_CORE_SCENE_MAIN_SCENE_H_

#include "scene.h"

namespace rendu {

class main_scene final : public scene {
 private:
  std::vector<entt::entity> m_entities;

 public:
  void on_enter(entt::registry &registry) override;
  void on_update(entt::registry &registry) override;
  void on_exit(entt::registry &registry) override;
};

}

#endif //RENDU_CORE_CORE_SCENE_MAIN_SCENE_H_
