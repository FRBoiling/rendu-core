/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_COMMON_COMPONENT_H
#define RENDU_COMMON_COMPONENT_H

#include <entt/entt.hpp>

namespace rendu {
    template< typename T>
    class Component: public entt::component_traits<T>{

    };
}

#endif //RENDU_COMMON_COMPONENT_H
