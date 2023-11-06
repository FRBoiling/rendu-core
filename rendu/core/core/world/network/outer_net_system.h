/*
* Created by boil on 2023/10/20.
*/

#ifndef RENDU_OUTER_NET_SYSTEM_H
#define RENDU_OUTER_NET_SYSTEM_H

#include "entity/component_system.h"
#include "common/utils/singleton.h"
#include "task/task.h"
#include "network/a_service.h"

RD_NAMESPACE_BEGIN

    class OuterNetSystem : public Singleton<OuterNetSystem>,
                           public ComponentSystem<void>,
                           public SystemAwake {

    public:
      void Awake() override;

    private:
      AService* m_service{};
    };

RD_NAMESPACE_END

#endif //RENDU_OUTER_NET_SYSTEM_H
