/*
* Created by boil on 2023/4/9.
*/

#include "controls.h"
#include "manager.h"

void input::controls::add_trigger(const std::string &name, binding_type bindings) {
    m_triggers.emplace(name, trigger_context{
            .bindings = std::move(bindings)
    });
}

void input::controls::add_axis(
        const std::string &name,
        binding_type left_bindings,
        binding_type right_bindings,
        binding_type up_bindings,
        binding_type down_bindings
) {
    m_axis.emplace(name, axis_context{
            .left_bindings  = std::move(left_bindings),
            .right_bindings = std::move(right_bindings),
            .up_bindings    = std::move(up_bindings),
            .down_bindings  = std::move(down_bindings)
    });
}

void input::controls::add_passthrough_vec2(
        const std::string &name,
        passthrough_vec2_source_type source
) {
    m_passthrough_vec2.emplace(name, passthrough_context<math::vec2>{
            .source = source
    });
}
