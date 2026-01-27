#pragma once

#include <string>
#include "core/define.h"

BEGIN_NAMESPACE_CORE

class State {
public:
    explicit State(std::string name);
    virtual ~State() = default;

    // 进入状态
    virtual void on_enter() {}

    // 离开状态
    virtual void on_exit() {}

    // 更新
    virtual void on_update(float delta_time) {}

    const std::string& name() const;

private:
    std::string name_;
};

END_NAMESPACE_CORE
