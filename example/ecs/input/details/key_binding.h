/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_KEY_BINDING_H
#define RENDU_KEY_BINDING_H

#include "binding_check.h"
#include "ImGuiIO.h"

namespace input {
    namespace details {
        template<uint8_t scancode>
        class key_binding final : public binding_check {
        public:
            bool check(entt::registry &registry, const state &state) const override {
                auto &io = registry.ctx().get<ImGuiIO &>();
                return (!io.WantCaptureKeyboard && state.keyboard_state[scancode]);
            }
        };
    }

    template<uint8_t scancode>
    binding_type key() {
        return std::make_unique<details::key_binding<scancode>>();
    }

}

#endif //RENDU_KEY_BINDING_H
