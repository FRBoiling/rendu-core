#pragma once

#include <string>
#include <vector>
#include "core/define.h"

BEGIN_NAMESPACE_CORE

class ILifecycle {
public:
    virtual ~ILifecycle() = default;

    // 初始化组件
    virtual void initialize() = 0;

    // 关闭组件
    virtual void shutdown() = 0;

    // 返回依赖的组件名称列表
    virtual std::vector<std::string> dependencies() const {
        return {};
    }

    // 获取组件名称
    const std::string& name() const { return name_; }

    // 设置组件名称（仅供 LifecycleManager 使用）
    void set_name(const std::string& name) { name_ = name; }

protected:
    std::string name_;
};

END_NAMESPACE_CORE
