/*
* Created by boil on 2023/4/10.
*/

#ifndef RENDU_PASSTHROUGH_SOURCE_H
#define RENDU_PASSTHROUGH_SOURCE_H

#include "details/ImGuiIO.h"
#include "state.h"
#include <math/math.pb.h>
#include <variant>
#include <entt/entt.hpp>

namespace input {
    template<typename T>
    class passthrough_source {
        static_assert(sizeof(T) == 0, "unsupported passthrough data type");
    };

    template<>
    class passthrough_source<math::vec2> {
    public:
        // 标记类型以标识源
        struct mouse_pointer {
        };
        struct mouse_delta {
        };
        struct mouse_wheel {
        };

        using type = std::variant<
                mouse_pointer,
                mouse_delta,
                mouse_wheel
        >;

    public:
        static void read(
                type source,
                entt::registry &registry,
                const state &state,
                math::vec2 &out
        ) {
            std::visit(
                    [&](auto &src) { read_source(src, registry, state, out); },
                    source
            );
        }

    private:
        static void read_source(
                mouse_pointer source,
                entt::registry &registry,
                const state &state,
                math::vec2 &out
        ) {
            auto &io = registry.ctx().get<ImGuiIO &>();
            if (!io.WantCaptureMouse) {
                out.set_x(state.mouse_x);
                out.set_y(state.mouse_y);
            }
        }

        static void read_source(
                mouse_delta source,
                entt::registry &registry,
                const state &state,
                math::vec2 &out
        ) {
            auto &io = registry.ctx().get<ImGuiIO &>();
            if (!io.WantCaptureMouse) {
                out.set_x(state.mouse_motion_x);
                out.set_y(state.mouse_motion_y);
            }
        }

        static void read_source(
                mouse_wheel source,
                entt::registry &registry,
                const state &state,
                math::vec2 &out
        ) {
            auto &io = registry.ctx().get<ImGuiIO &>();
            if (!io.WantCaptureMouse) {
                out.set_x(state.mouse_wheel_x);
                out.set_y(state.mouse_wheel_y);
            }
        }
    };
}

#endif //RENDU_PASSTHROUGH_SOURCE_H
