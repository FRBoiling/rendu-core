/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_BINDING_CHECK_H
#define RENDU_BINDING_CHECK_H

#include <entt/entt.hpp>
#include "state.h"

namespace input {

    namespace details {
        class binding_check {
        public:
            virtual ~binding_check() { };

            virtual bool check(entt::registry &registry, const state &state) const {
                return false;
            }
        };
    };

}
#endif //RENDU_BINDING_CHECK_H
