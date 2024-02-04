/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_COMPONENT_SYSTEM_H
#define RENDU_COMPONENT_SYSTEM_H

#include "core_define.h"

RD_NAMESPACE_BEGIN

    template<typename Component>
    class ComponentSystem {
    public:
      ComponentSystem() = default;
      virtual ~ComponentSystem() = default;
    };

    class SystemAwake {
    public:
      virtual void Awake() {};
    };

    template<typename ... Args>
    class SystemArgsAwake {
    public:
      virtual void Awake(Args ...args) {};
    };

    class SystemUpdate {
    public:
      virtual void Update() {};
    };

    class SystemLateUpdate {
    public:
      virtual void LateUpdate() {};
    };

RD_NAMESPACE_END
#endif //RENDU_COMPONENT_SYSTEM_H
