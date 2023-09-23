/*
* Created by boil on 2023/9/19.
*/

#ifndef RENDU_WORLD_H
#define RENDU_WORLD_H

#include <typeindex>
#include <unordered_map>
#include "ecs/entity.h"
#include "entity/component_system.h"
#include "log/logger.h"
#include "base/singleton.h"

namespace rendu {

    class World {
    public:
      World() {
        m_entity = m_registry.create();
      };

    private:
      ~World() {
        m_registry.destroy(m_entity);
      };

    public:
      static World &Instance() {
        static World object;
        return object;
      }

    public :
      Entity &GetEntity() {
        return m_entity;
      }

      template<typename System>
      void AddSingleton() {
        auto &system = System::Instance();
        system.Init(m_registry);
        system.Awake();
        RD_INFO(" world add singleton {} awake", typeid(System).name());
      }

      template<typename System, typename... Args>
      void AddSingleton(Args ...args) {
        auto &system = System::Instance();
        system.Init(m_registry);
        system.Awake(args...);
        RD_INFO(" world add singleton {} awake , args", typeid(System).name());
      }

    private:
      EntityPool m_registry;
      Entity m_entity;

    };
}// namespace rendu

#endif //RENDU_WORLD_H
