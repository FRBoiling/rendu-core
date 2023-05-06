/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_ENTITY_H
#define RENDU_ENTITY_H

#include <entt/entt.hpp>

namespace rendu {

    class Entity : entt::registry {
        using entity_type = entt::entity;
    };

}

#endif //RENDU_ENTITY_H
