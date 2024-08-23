/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_CORE_CORE_ENTITY_I_SCENE_H_
#define RENDU_CORE_CORE_ENTITY_I_SCENE_H_

#include "core_define.h"

RD_NAMESPACE_BEGIN
class Fiber;

class IScene {
  public:
    IScene(Fiber *fiber, Int sceneType): m_fiber(fiber), m_sceneType(sceneType) {
    }

  protected:
    Fiber *m_fiber;
    Int m_sceneType;
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_ENTITY_I_SCENE_H_
