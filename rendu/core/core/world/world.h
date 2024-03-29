/*
* Created by boil on 2023/9/19.
*/

#ifndef RENDU_WORLD_H
#define RENDU_WORLD_H

#include "fwd/core_fwd.h"

#include "base/utils/singleton.h"
#include "ecs/entity.h"
#include "entity/component_system.h"
#include <typeindex>
#include <unordered_map>

RD_NAMESPACE_BEGIN

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
        RD_TRACE("AddSingleton success. {} Awake .", typeid(System).name());
      }

      template<typename System, typename... Args>
      void AddSingleton(Args ...args) {
        auto &system = System::Instance();
        system.Init(m_registry);
        system.Awake(args...);
        RD_TRACE("AddSingleton with args success. {} Awake .", typeid(System).name());
      }

    private:
      EntityPool m_registry;
      Entity m_entity;

    };

RD_NAMESPACE_END

#endif //RENDU_WORLD_H
