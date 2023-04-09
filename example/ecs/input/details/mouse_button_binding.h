/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_MOUSE_BUTTON_BINDING_H
#define RENDU_MOUSE_BUTTON_BINDING_H

#include "binding_check.h"
#include "ImGuiIO.h"

namespace input::details {

    template<uint32_t buttonmask>
    class mouse_button_binding final : public binding_check {
    public:
        virtual bool check(entt::registry &registry, const state &state) const {
            auto &io = registry.ctx().get<ImGuiIO &>();
            return (!io.WantCaptureMouse && (state.mouse_button_mask & buttonmask) != 0);
        }
    };
}

#endif //RENDU_MOUSE_BUTTON_BINDING_H
