/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_COMMON_ENTITY_POOL_H
#define RENDU_COMMON_ENTITY_POOL_H

#include "entity.h"

COMMON_NAMESPACE_BEGIN

class EntityPool : public ENTITY_POOL {
public:
  template<typename Component>
  auto GetEntities() {
    entt::registry registry;
    return registry.view<Component>();
  }
};

COMMON_NAMESPACE_END

#endif//RENDU_COMMON_ENTITY_POOL_H
