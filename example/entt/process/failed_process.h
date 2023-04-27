/*
* Created by boil on 2023/4/28.
*/

#ifndef RENDU_FAILED_PROCESS_H
#define RENDU_FAILED_PROCESS_H

#include <entt/entt.hpp>

struct failed_process: entt::process<failed_process, int> {
    void update(delta_type, void *) {
        ++invoked;
        fail();
    }

    static inline unsigned int invoked;
};


#endif //RENDU_FAILED_PROCESS_H
