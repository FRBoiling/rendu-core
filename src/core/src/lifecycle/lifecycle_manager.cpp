#include <core/lifecycle/lifecycle_manager.h>
#include <common/log/logger.h>
#include <algorithm>
#include <stack>
#include <queue>

using namespace Rendu;
using namespace Rendu::log;

LifecycleManager::~LifecycleManager() {
    shutdown_all();
}

void LifecycleManager::register_component(const std::string& name, std::shared_ptr<ILifecycle> component) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!component) {
        throw std::invalid_argument("Cannot register null component");
    }

    if (initialized_) {
        throw std::runtime_error("Cannot register components after initialization");
    }

    if (components_.find(name) != components_.end()) {
        RENDU_LOG_WARN("LifecycleManager: Component '{}' already registered, will be replaced", name);
    }

    component->set_name(name);
    components_[name] = component;

    RENDU_LOG_INFO("LifecycleManager: Registered component '{}'", name);
}

void LifecycleManager::initialize_all() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        RENDU_LOG_WARN("LifecycleManager: Already initialized");
        return;
    }

    if (components_.empty()) {
        RENDU_LOG_INFO("LifecycleManager: No components to initialize");
        initialized_ = true;
        return;
    }

    RENDU_LOG_INFO("LifecycleManager: Starting initialization of {} components", components_.size());

    // 拓扑排序确定初始化顺序
    std::vector<std::string> init_order = topological_sort();

    if (init_order.empty()) {
        RENDU_LOG_ERROR("LifecycleManager: Failed to determine initialization order (possibly circular dependency)");
        throw std::runtime_error("Circular dependency detected");
    }

    // 按拓扑顺序初始化组件
    for (const auto& name : init_order) {
        auto it = components_.find(name);
        if (it != components_.end()) {
            try {
                RENDU_LOG_INFO("LifecycleManager: Initializing component '{}'", name);
                it->second->initialize();
                RENDU_LOG_INFO("LifecycleManager: Component '{}' initialized successfully", name);
            } catch (const std::exception& e) {
                RENDU_LOG_ERROR("LifecycleManager: Failed to initialize component '{}': {}", name, e.what());
                // 初始化失败，尝试关闭已初始化的组件
                shutdown_unsafe();
                throw;
            }
        }
    }

    initialized_ = true;
    init_order_ = init_order; // 保存初始化顺序
    RENDU_LOG_INFO("LifecycleManager: All components initialized successfully");
}

void LifecycleManager::shutdown_all() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        RENDU_LOG_INFO("LifecycleManager: Not initialized, nothing to shutdown");
        return;
    }

    if (shutting_down_) {
        RENDU_LOG_WARN("LifecycleManager: Already shutting down");
        return;
    }

    shutting_down_ = true;

    try {
        shutdown_unsafe();
        initialized_ = false;
        shutting_down_ = false;
        RENDU_LOG_INFO("LifecycleManager: All components shutdown successfully");
    } catch (...) {
        shutting_down_ = false;
        throw;
    }
}

void LifecycleManager::shutdown_unsafe() {
    // 按初始化的逆序关闭组件
    // 如果还没有保存初始化顺序，使用当前所有组件的逆序
    std::vector<std::string> shutdown_order;

    if (init_order_.empty()) {
        // 还没有保存初始化顺序，使用当前所有组件
        for (const auto& pair : components_) {
            shutdown_order.push_back(pair.first);
        }
        std::reverse(shutdown_order.begin(), shutdown_order.end());
    } else {
        // 使用保存的初始化顺序
        shutdown_order = init_order_;
        std::reverse(shutdown_order.begin(), shutdown_order.end());
    }

    for (const auto& name : shutdown_order) {
        auto it = components_.find(name);
        if (it != components_.end()) {
            try {
                RENDU_LOG_INFO("LifecycleManager: Shutting down component '{}'", name);
                it->second->shutdown();
                RENDU_LOG_INFO("LifecycleManager: Component '{}' shutdown successfully", name);
            } catch (const std::exception& e) {
                RENDU_LOG_ERROR("LifecycleManager: Failed to shutdown component '{}': {}", name, e.what());
                // 继续关闭其他组件
            }
        }
    }
}

bool LifecycleManager::has_component(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return components_.find(name) != components_.end();
}

std::vector<std::string> LifecycleManager::get_component_names() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(components_.size());
    for (const auto& pair : components_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> LifecycleManager::topological_sort() {
    // 构建依赖图
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, int> in_degree;

    // 初始化所有节点
    for (const auto& pair : components_) {
        const std::string& name = pair.first;
        graph[name] = std::vector<std::string>();
        in_degree[name] = 0;
    }

    // 添加依赖边
    for (const auto& pair : components_) {
        const std::string& name = pair.first;
        std::vector<std::string> deps = pair.second->dependencies();

        for (const auto& dep : deps) {
            // 检查依赖是否存在
            if (components_.find(dep) != components_.end()) {
                graph[dep].push_back(name);
                in_degree[name]++;
            } else {
                RENDU_LOG_WARN("LifecycleManager: Component '{}' depends on non-existent component '{}'", name, dep);
            }
        }
    }

    // 检查循环依赖
    if (has_cyclic_dependency(graph)) {
        RENDU_LOG_ERROR("LifecycleManager: Circular dependency detected");
        return {};
    }

    // Kahn 算法进行拓扑排序
    std::queue<std::string> queue;
    std::vector<std::string> result;

    // 将入度为 0 的节点加入队列
    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            queue.push(pair.first);
        }
    }

    while (!queue.empty()) {
        std::string node = queue.front();
        queue.pop();
        result.push_back(node);

        // 减少邻接节点的入度
        for (const auto& neighbor : graph[node]) {
            in_degree[neighbor]--;
            if (in_degree[neighbor] == 0) {
                queue.push(neighbor);
            }
        }
    }

    // 如果结果数不等于节点数，说明有循环依赖
    if (result.size() != components_.size()) {
        RENDU_LOG_ERROR("LifecycleManager: Circular dependency detected (not all components can be initialized)");
        return {};
    }

    return result;
}

bool LifecycleManager::has_cyclic_dependency(const std::unordered_map<std::string, std::vector<std::string>>& graph) {
    std::unordered_map<std::string, int> state; // 0: 未访问, 1: 访问中, 2: 已完成

    for (const auto& pair : graph) {
        state[pair.first] = 0;
    }

    std::function<bool(const std::string&)> dfs = [&](const std::string& node) -> bool {
        if (state[node] == 1) {
            // 找到环
            return true;
        }
        if (state[node] == 2) {
            // 已完成，无环
            return false;
        }

        state[node] = 1; // 标记为访问中

        for (const auto& neighbor : graph.at(node)) {
            if (dfs(neighbor)) {
                return true;
            }
        }

        state[node] = 2; // 标记为已完成
        return false;
    };

    for (const auto& pair : graph) {
        if (state[pair.first] == 0) {
            if (dfs(pair.first)) {
                return true;
            }
        }
    }

    return false;
}
