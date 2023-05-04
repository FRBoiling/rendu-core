/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_COMPONENT_H
#define RENDU_COMPONENT_H

#include <entt/entt.hpp>

namespace rendu {
    template< typename T>
    class Component: public entt::component_traits<T>{

    };
}

#endif //RENDU_COMPONENT_H
