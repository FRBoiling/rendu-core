#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <core/state/state.h>
#include "core/define.h"

BEGIN_NAMESPACE_CORE

using TransitionCondition = std::function<bool()>;

class StateMachine {
public:
    StateMachine() = default;
    ~StateMachine() = default;

    // 注册状态
    void register_state(std::shared_ptr<State> state);

    // 设置初始状态
    void set_initial_state(const std::string& name);

    // 添加状态转换
    void add_transition(
        const std::string& from,
        const std::string& to,
        TransitionCondition condition = nullptr
    );

    // 启动
    void start();

    // 手动切换状态
    void change_state(const std::string& name);

    // 更新
    void update(float delta_time);

    // 查询
    State* current_state() const;
    bool has_state(const std::string& name) const;

private:
    bool can_transition(const std::string& to) const;

    std::unordered_map<std::string, std::shared_ptr<State>> states_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, TransitionCondition>>> transitions_;
    std::string initial_state_;
    State* current_state_{nullptr};
    bool started_{false};
};

END_NAMESPACE_CORE
