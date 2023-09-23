/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_FIBER_MANAGER_SYSTEM_H
#define RENDU_FIBER_MANAGER_SYSTEM_H

#include "define.h"
#include "entity/component_system.h"
#include "fiber_pool.h"
#include "base/singleton.h"

RD_NAMESPACE_BEGIN

    class FiberManagerSystem
        : public Singleton<FiberManagerSystem>,
          public ComponentSystem<FiberPool>,
          public SystemAwake,
          public SystemUpdate,
          public SystemLateUpdate {

    public:
      void Awake() override;

      void Update() override;

      void LateUpdate() override;
    };

RD_NAMESPACE_END

#endif //RENDU_FIBER_MANAGER_SYSTEM_H
