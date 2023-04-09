/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_STATE_H
#define RENDU_STATE_H


#include <cstdint>

namespace input {
    struct state {
        const uint8_t *keyboard_state;

        uint32_t mouse_button_mask;
        int mouse_x;
        int mouse_y;
        int mouse_motion_x;
        int mouse_motion_y;
        int mouse_wheel_x;
        int mouse_wheel_y;
    };
}
#endif //RENDU_STATE_H
