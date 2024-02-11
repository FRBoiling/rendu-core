/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_ENTITY_SYSTEM_H
#define RENDU_ENTITY_SYSTEM_H

#include "core_define.h"

CORE_NAMESPACE_BEGIN

    class EntitySystem {

    public:
      void Update();

      void LateUpdate();
    };

CORE_NAMESPACE_END

#endif //RENDU_ENTITY_SYSTEM_H
