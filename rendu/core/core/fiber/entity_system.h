/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_ENTITY_SYSTEM_H
#define RENDU_ENTITY_SYSTEM_H

#include "core_define.h"
#include "ecs/entity.h"

RD_NAMESPACE_BEGIN
class EntitySystem {
  public:
    EntitySystem();
    ~EntitySystem();

    template<typename T>
    void Publish(T t) {
    }

    std::queue<std::shared_ptr<Entity> > &GetQueue(Type type);

    void RegisterSystem(Entity component);

  private:
    std::unordered_map<Type, std::queue<std::shared_ptr<Entity> > > queues;
};

RD_NAMESPACE_END

#endif//RENDU_ENTITY_SYSTEM_H
