#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <functional>
#include <core/lifecycle/lifecycle.h>
#include "core/define.h"

BEGIN_NAMESPACE_CORE

class LifecycleManager {
public:
    LifecycleManager() = default;
    ~LifecycleManager();

    // 注册组件
    void register_component(const std::string& name, std::shared_ptr<ILifecycle> component);

    // 初始化所有组件
    void initialize_all();

    // 关闭所有组件
    void shutdown_all();

    // 获取组件
    template<typename T>
    T* get(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = components_.find(name);
        if (it == components_.end()) {
            return nullptr;
        }
        return dynamic_cast<T*>(it->second.get());
    }

    // 检查组件是否存在
    bool has_component(const std::string& name) const;

    // 检查是否已初始化
    bool is_initialized() const { return initialized_; }

    // 获取所有组件名称
    std::vector<std::string> get_component_names() const;

private:
    // 拓扑排序
    std::vector<std::string> topological_sort();

    // 检测循环依赖
    bool has_cyclic_dependency(const std::unordered_map<std::string, std::vector<std::string>>& graph);

    // 不安全的关闭（不加锁，内部使用）
    void shutdown_unsafe();

    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<ILifecycle>> components_;
    bool initialized_{false};
    bool shutting_down_{false};
    std::vector<std::string> init_order_; // 保存初始化顺序
};

END_NAMESPACE_CORE
