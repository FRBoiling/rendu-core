/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_INPUT_SYSTEM_H
#define RENDU_INPUT_SYSTEM_H

#include "base/system.h"

class InputSystem : public System{
public:
    void run(entt::registry &registry) override;

};


#endif //RENDU_INPUT_SYSTEM_H
