/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_CONTROLS_H
#define RENDU_CONTROLS_H

#include "helper.h"
#include "action.h"
#include "passthrough_source.h"
#include <math/math.pb.h>

namespace input {
    using passthrough_vec2_source_type = typename passthrough_source<math::vec2>::type;

    /* forward */ class manager;

    class controls {
        friend class manager;

    public:
        struct trigger_context {
            action_trigger value;
            binding_type bindings;
        };

        struct axis_context {
            action_axis value;
            binding_type left_bindings;
            binding_type right_bindings;
            binding_type up_bindings;
            binding_type down_bindings;
        };

        template<typename T>
        struct passthrough_context {
            using source_type = typename passthrough_source<T>::type;

            action_passthrough<T> value;
            source_type source;
        };


    public:
        controls() = default;

        controls(const controls &) = delete;

    private:
        std::unordered_map<std::string, trigger_context> m_triggers;
        std::unordered_map<std::string, axis_context> m_axis;
        std::unordered_map<std::string, passthrough_context<math::vec2>> m_passthrough_vec2;

    public:
        void add_trigger(const std::string &name, binding_type bindings);

        void add_axis(
                const std::string &name,
                binding_type left_bindings,
                binding_type right_bindings,
                binding_type up_bindings,
                binding_type down_bindings
        );

        void add_passthrough_vec2(
                const std::string &name,
                passthrough_vec2_source_type source
        );
    };

} // namespace input;

#endif //RENDU_CONTROLS_H
