/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_COMMON_ENTITY_POOL_H
#define RENDU_COMMON_ENTITY_POOL_H

#include "entity.h"

namespace rendu {

    class EntityPool: public entt::registry {
    public:
      template<typename Component>
      auto GetEntities(){
        entt::registry registry;
        return registry.view<Component>();
      }
    };
}

#endif //RENDU_COMMON_ENTITY_POOL_H
