/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_MANAGER_H
#define RENDU_MANAGER_H

#include <SDL.h>
#include "controls.h"
#include "state.h"

namespace input {
    class manager {
    private:
        controls m_controls;
        state m_state;

    public:
        // 由于m_controls，管理器也是不可复制的
        manager() = default;
        manager(const manager &) = delete;

        void configure(std::function<void(controls &)> fn);

        // 有关这些方法的实现，请参见上面
        void process_event(SDL_Event *event);
        void frame_begin();
        void update(entt::registry &registry);

        // ...
    public:
        const action_trigger &read_trigger(const std::string &name) {
            return m_controls.m_triggers.at(name).value;
        }

        const action_axis &read_axis(const std::string &name) {
            return m_controls.m_axis.at(name).value;
        }

        const action_passthrough<math::vec2> &read_passthrough_vec2(const std::string &name) {
            return m_controls.m_passthrough_vec2.at(name).value;
        }
    };
}


#endif //RENDU_MANAGER_H
