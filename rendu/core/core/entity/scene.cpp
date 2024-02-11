/*
* Created by boil on 2023/9/26.
*/

#include "scene.h"
#include "fiber/fiber.h"

CORE_NAMESPACE_BEGIN

    Scene::Scene(Fiber &fiber, Long id, Long instanceId, SceneType sceneType, std::string& name) {
      m_id = id;
      m_name = name;
      m_instanceId = instanceId;
      m_type = sceneType;
      m_isCreate = true;
      m_isNew = true;
      m_fiber = &fiber;
      m_isRegister = true;
    }

CORE_NAMESPACE_END
