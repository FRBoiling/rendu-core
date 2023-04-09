/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_MESSAGING_SYSTEM_H
#define RENDU_MESSAGING_SYSTEM_H

#include "base/system.h"

class MessagingSystem : public System {
public:
    void run(entt::registry &registry) override;

};


#endif //RENDU_MESSAGING_SYSTEM_H
