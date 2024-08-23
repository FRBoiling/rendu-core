/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_CORE_CORE_ENTITY_SCENE_H_
#define RENDU_CORE_CORE_ENTITY_SCENE_H_

#include <utility>

#include "i_scene.h"
#include "entity.h"

RD_NAMESPACE_BEGIN
class Scene : public Entity, public IScene {
  public:
    Scene(Fiber *fiber, Int sceneType, Long instanceId, Long id, String name): IScene(fiber, sceneType),
                                                                               Entity(instanceId, id),
                                                                               m_name(std::move(name)) {
    }

  private:
    String m_name;
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_ENTITY_SCENE_H_
