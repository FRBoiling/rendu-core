/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_COMPONENT_SYSTEM_H
#define RENDU_SYSTEM_H

#include <entt/entt.hpp>

enum class ControlFlow {
    exit,
    loop
};

class System {
public:
    System() = default;
    virtual ~System() = default;

public:
    virtual void run(entt::registry &registry) = 0;
};

#endif //RENDU_COMPONENT_SYSTEM_H
