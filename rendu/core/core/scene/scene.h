/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_SCENE_SCENE_H_
#define RENDU_CORE_CORE_SCENE_SCENE_H_

#include <entt/entt.hpp>

namespace rendu {

class scene {
 public:
  virtual void on_enter(entt::registry &registry) = 0;
  virtual void on_update(entt::registry &registry) = 0;
  virtual void on_exit(entt::registry &registry) = 0;
};

}

#endif //RENDU_CORE_CORE_SCENE_SCENE_H_
