/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_SCHEDULER_H
#define RENDU_SCHEDULER_H

#include <entt/entt.hpp>

namespace rendu {

    template<typename Delta>
    class Scheduler : public entt::scheduler<Delta> {

    };
}


#endif //RENDU_SCHEDULER_H
