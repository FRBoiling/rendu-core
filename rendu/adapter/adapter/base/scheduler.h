/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_SCHEDULER_H
#define RENDU_SCHEDULER_H

#include <entt/entt.hpp>

namespace rendu {

    class Scheduler : public entt::scheduler<int> {
    public:
      virtual void Add(int fiberId) {
      };
    };
}


#endif //RENDU_SCHEDULER_H
