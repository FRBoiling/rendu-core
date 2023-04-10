/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_MOUSE_BUTTON_BINDING_H
#define RENDU_MOUSE_BUTTON_BINDING_H

#include "binding_check.h"

namespace input {
    namespace details {
        template<uint32_t buttonmask>
        class mouse_button_binding final : public binding_check {
        public:
            bool check(entt::registry &registry, const state &state) const override {
                return state.mouse_button_mask & buttonmask;
            }
        };
    }

    template<uint32_t buttonmask>
    binding_type mouse_button() {
        return std::make_unique<details::mouse_button_binding<buttonmask>>();
    }
}
#endif //RENDU_MOUSE_BUTTON_BINDING_H
