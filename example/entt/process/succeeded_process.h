/*
* Created by boil on 2023/4/28.
*/

#ifndef RENDU_SUCCEEDED_PROCESS_H
#define RENDU_SUCCEEDED_PROCESS_H
#include <entt/entt.hpp>

struct succeeded_process: entt::process<succeeded_process, int> {
    void update(delta_type, void *) {
        ++invoked;
        succeed();
    }

    static inline unsigned int invoked;
};


#endif //RENDU_SUCCEEDED_PROCESS_H
