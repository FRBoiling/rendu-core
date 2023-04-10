/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_SCENE_H
#define RENDU_SCENE_H

#include <entt/entt.hpp>

class scene {
public:
    scene() = default;
    virtual ~scene() = default;
public:
    virtual void on_enter(entt::registry &registry) = 0;
    virtual void on_update(entt::registry &registry) = 0;
    virtual void on_exit(entt::registry &registry) = 0;
};
#endif //RENDU_SCENE_H
