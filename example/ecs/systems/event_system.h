/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_EVENT_SYSTEM_H
#define RENDU_EVENT_SYSTEM_H

#include "base/system.h"

class EventSystem final : public System {
public:
    void run(entt::registry &registry) override;
};


#endif //RENDU_EVENT_SYSTEM_H
