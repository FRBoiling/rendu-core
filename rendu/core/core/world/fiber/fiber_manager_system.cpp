/*
* Created by boil on 2023/9/22.
*/

#include "fiber_manager_system.h"

RD_NAMESPACE_BEGIN

    void FiberManagerSystem::Update() {
      SystemUpdate::Update();
    }

    void FiberManagerSystem::Awake() {
      SystemAwake::Awake();
    }

    void FiberManagerSystem::LateUpdate() {
      SystemLateUpdate::LateUpdate();
    }


RD_NAMESPACE_END


