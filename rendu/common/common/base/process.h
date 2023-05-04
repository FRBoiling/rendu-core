/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_PROCESS_H
#define RENDU_PROCESS_H

#include <entt/entt.hpp>
#include "log/logger.h"

namespace rendu {

    template<typename Derived, typename Delta>
    class Process : public entt::process<Derived, Delta> {

    };

}


#endif //RENDU_PROCESS_H
