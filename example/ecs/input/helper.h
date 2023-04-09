/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_HELPER_H
#define RENDU_HELPER_H

#include "details/key_binding.h"
#include "details/mouse_button_binding.h"
#include "details/binding_or_combinator.h"
#include "details/binding_and_combinator.h"

namespace input {
    using binding_type = std::unique_ptr<details::binding_check>;

    binding_type nobinding() {
        return std::make_unique<details::binding_check>();
    }

    template <uint8_t scancode>
    binding_type key() {
        return std::make_unique<details::key_binding<scancode>>();
    }

    template <uint32_t buttonmask>
    binding_type mouse_button() {
        return std::make_unique<details::mouse_button_binding<buttonmask>>();
    }
}

input::binding_type operator|(
        input::binding_type a,
        input::binding_type b
) {
    return std::make_unique<input::details::binding_or_combinator>(
            std::move(a),
            std::move(b)
    );
}

input::binding_type operator&(
        input::binding_type a,
        input::binding_type b
) {
    return std::make_unique<input::details::binding_and_combinator>(
            std::move(a),
            std::move(b)
    );
}
#endif //RENDU_HELPER_H
