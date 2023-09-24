/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_EVENT_SYSTEM_H
#define RENDU_EVENT_SYSTEM_H

#include "entity/component_system.h"
#include "singleton.h"
#include "event.h"
#include "task/task.h"
#include "fiber/fiber_init.h"
#include "invoke_handler.h"
#include <typeinfo>
#include <any>

RD_NAMESPACE_BEGIN

    class EventSystem
        : public Singleton<EventSystem>,
          public ComponentSystem<EventInfo>,
          public SystemAwake {

    public:
      void Awake() override;

    public:
      template<typename A, typename T>
      Task<T> Invoke(long type, A args) {
        co_return;
      }

    private:
      std::unordered_map<std::type_index, std::list<EventInfo>> allEvents;
      std::unordered_map<std::type_index, std::unordered_map<long, std::any>> allInvokers;
    };


RD_NAMESPACE_END

#endif //RENDU_EVENT_SYSTEM_H
