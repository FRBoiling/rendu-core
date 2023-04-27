/*
* Created by boil on 2023/4/28.
*/

#ifndef RENDU_FOO_PROCESS_H
#define RENDU_FOO_PROCESS_H
#include <entt/entt.hpp>


struct foo_process: entt::process<foo_process, int> {
    foo_process(std::function<void()> upd, std::function<void()> abort)
            : on_update{upd}, on_aborted{abort} {}

    void update(delta_type, void *) {
        on_update();
    }

    void aborted() {
        on_aborted();
    }

    std::function<void()> on_update;
    std::function<void()> on_aborted;
};


#endif //RENDU_FOO_PROCESS_H
