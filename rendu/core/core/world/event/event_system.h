/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_EVENT_SYSTEM_H
#define RENDU_EVENT_SYSTEM_H

#include "base/utils/singleton.h"
#include "entity/component_system.h"
#include "event_info.h"
#include "exception/exception_helper.h"
#include "invoke_handler.h"
#include <any>
#include <typeinfo>
#include <typeindex>

CORE_NAMESPACE_BEGIN

    class EventSystem
        : public Singleton<EventSystem>,
          public ComponentSystem<EventInfo>,
          public SystemAwake {

    public:
      void Awake() override;

    public:
      template<typename A, typename T>
      Task<T> Invoke(Long type, A args) {
        CDictionary<Long, std::any> invokeHandlers;
        if (!allInvokers.TryGetValue(typeid(A), invokeHandlers)) {
          throw Exception("Invoke error4: {} {}", type, typeid(A).name());
        }

        std::any invokeHandler;
        if (!invokeHandlers.TryGetValue(type, invokeHandler)) {
          throw Exception("Invoke error4: {} {}", type, typeid(A).name());
        }

        auto aInvokeHandler = std::any_cast<AInvokeHandler<A, T> *>(invokeHandler);
        if (aInvokeHandler == nullptr) {
          throw Exception("Invoke error4: {} {}", type, typeid(A).name(), typeid(T).name());
        }
        co_return aInvokeHandler->Handle(args);
      }

    private:
      CDictionary<std::type_index, std::list<EventInfo>> allEvents;
      CDictionary<std::type_index, CDictionary<Long, std::any>> allInvokers;
    };


CORE_NAMESPACE_END

#endif //RENDU_EVENT_SYSTEM_H
