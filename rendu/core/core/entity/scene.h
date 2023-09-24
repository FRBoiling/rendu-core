/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_SCENE_H
#define RENDU_SCENE_H

#include "define.h"
#include "scene_type.h"

RD_NAMESPACE_BEGIN

    class Fiber;

    class Scene {
    public:
      Scene(Fiber& fiber, long id, long instanceId, SceneType sceneType, std::string& name);

    public:
      int64_t m_id;
      std::string m_name;
      int64_t m_instanceId;
      SceneType m_type;
      bool m_isCreate;
      bool m_isNew;
      Fiber* m_fiber;
      bool m_isRegister;
    };

RD_NAMESPACE_END

#endif //RENDU_SCENE_H
