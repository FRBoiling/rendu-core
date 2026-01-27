#include "core/state/state_machine.h"
#include <stdexcept>
#include "core/define.h"

BEGIN_NAMESPACE_CORE

void StateMachine::register_state(std::shared_ptr<State> state) {
    if (!state) {
        throw std::invalid_argument("State cannot be null");
    }

    const auto& name = state->name();
    if (states_.contains(name)) {
        // 静默替换同名状态
    }

    states_[name] = state;
}

void StateMachine::set_initial_state(const std::string& name) {
    if (!states_.contains(name)) {
        throw std::invalid_argument("Initial state '" + name + "' not registered");
    }
    initial_state_ = name;
}

void StateMachine::add_transition(
    const std::string& from,
    const std::string& to,
    TransitionCondition condition
) {
    if (!states_.contains(from)) {
        throw std::invalid_argument("Source state '" + from + "' not registered");
    }
    if (!states_.contains(to)) {
        throw std::invalid_argument("Target state '" + to + "' not registered");
    }

    transitions_[from].emplace_back(to, condition);
}

void StateMachine::start() {
    if (started_) {
        return;
    }

    if (initial_state_.empty()) {
        throw std::runtime_error("Initial state not set");
    }

    auto it = states_.find(initial_state_);
    if (it == states_.end()) {
        throw std::runtime_error("Initial state '" + initial_state_ + "' not found");
    }

    current_state_ = it->second.get();
    current_state_->on_enter();
    started_ = true;
}

void StateMachine::change_state(const std::string& name) {
    if (!started_) {
        return;
    }

    auto it = states_.find(name);
    if (it == states_.end()) {
        return;
    }

    // 离开当前状态
    if (current_state_) {
        current_state_->on_exit();
    }

    // 进入新状态
    current_state_ = it->second.get();
    current_state_->on_enter();
}

void StateMachine::update(float delta_time) {
    if (!started_ || !current_state_) {
        return;
    }

    // 更新当前状态
    current_state_->on_update(delta_time);

    // 检查是否可以转换
    const auto& current_name = current_state_->name();
    auto trans_it = transitions_.find(current_name);
    if (trans_it != transitions_.end()) {
        for (const auto& [target, condition] : trans_it->second) {
            // 如果没有条件或条件满足，则转换
            if (!condition || condition()) {
                change_state(target);
                break;
            }
        }
    }
}

State* StateMachine::current_state() const {
    return current_state_;
}

bool StateMachine::has_state(const std::string& name) const {
    return states_.contains(name);
}

bool StateMachine::can_transition(const std::string& to) const {
    if (!current_state_) {
        return false;
    }

    const auto& current_name = current_state_->name();
    auto trans_it = transitions_.find(current_name);
    if (trans_it == transitions_.end()) {
        return false;
    }

    for (const auto& [target, condition] : trans_it->second) {
        if (target == to) {
            return !condition || condition();
        }
    }

    return false;
}

END_NAMESPACE_CORE
