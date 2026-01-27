#include "core/state/state.h"
#include "core/define.h"

BEGIN_NAMESPACE_CORE

State::State(std::string name)
    : name_(std::move(name))
{}

const std::string& State::name() const {
    return name_;
}

END_NAMESPACE_CORE
