#include "core/cluster/failover_manager.h"

#include <iostream>
#include <algorithm>

BEGIN_NAMESPACE_CORE

FailoverManager::FailoverManager(const Config& config)
    : config_(config) {
}

FailoverManager::~FailoverManager() {
    stop();
}

void FailoverManager::start() {
    if (running_.load()) {
        return;
    }

    running_.store(true);
    health_check_thread_ = std::make_unique<std::thread>(&FailoverManager::health_check_loop, this);

    std::cout << "[FailoverManager] Started" << std::endl;
}

void FailoverManager::stop() {
    if (!running_.load()) {
        return;
    }

    running_.store(false);

    if (health_check_thread_ && health_check_thread_->joinable()) {
        health_check_thread_->join();
    }

    health_check_thread_.reset();

    std::cout << "[FailoverManager] Stopped" << std::endl;
}

void FailoverManager::add_node(const LoadBalancedNode& node) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    nodes_[node.node_id] = node;

    NodeHealth health;
    health.node_id = node.node_id;
    health.state = NodeState::Healthy;
    health.last_check = std::chrono::system_clock::now();
    health.consecutive_failures = 0;

    node_health_[node.node_id] = health;
}

void FailoverManager::remove_node(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    nodes_.erase(node_id);
    node_health_.erase(node_id);
}

void FailoverManager::mark_node_failed(const std::string& node_id, const std::string& reason) {
    update_node_state(node_id, NodeState::Failed, reason);
}

void FailoverManager::mark_node_recovered(const std::string& node_id) {
    update_node_state(node_id, NodeState::Healthy, "Recovered manually");
}

std::optional<NodeHealth> FailoverManager::get_node_health(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto it = node_health_.find(node_id);
    if (it != node_health_.end()) {
        return it->second;
    }

    return std::nullopt;
}

std::vector<LoadBalancedNode> FailoverManager::get_healthy_nodes() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    std::vector<LoadBalancedNode> result;
    for (const auto& [id, health] : node_health_) {
        if (health.state == NodeState::Healthy) {
            auto it = nodes_.find(id);
            if (it != nodes_.end()) {
                result.push_back(it->second);
            }
        }
    }

    return result;
}

std::vector<NodeHealth> FailoverManager::get_failed_nodes() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    std::vector<NodeHealth> result;
    for (const auto& [id, health] : node_health_) {
        if (health.state == NodeState::Failed) {
            result.push_back(health);
        }
    }

    return result;
}

size_t FailoverManager::get_node_count() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    return nodes_.size();
}

size_t FailoverManager::get_healthy_node_count() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    size_t count = 0;
    for (const auto& [id, health] : node_health_) {
        if (health.state == NodeState::Healthy) {
            count++;
        }
    }

    return count;
}

void FailoverManager::on_node_failure(NodeFailureCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    failure_callback_ = std::move(callback);
}

void FailoverManager::perform_health_check() {
    // 先收集需要检查的节点ID
    std::vector<std::string> node_ids_to_check;
    {
        std::lock_guard<std::mutex> lock(nodes_mutex_);
        for (const auto& [id, health] : node_health_) {
            if (health.state == NodeState::Healthy) {
                node_ids_to_check.push_back(id);
            }
        }
    }

    // 在锁外执行健康检查和状态更新
    for (const auto& id : node_ids_to_check) {
        bool is_healthy = check_node_health(id);
        if (!is_healthy) {
            NodeState new_state = NodeState::Unhealthy;
            update_node_state(id, new_state, "Health check failed");
        }
    }
}

void FailoverManager::set_health_checker(std::function<bool(const LoadBalancedNode&)> checker) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    health_checker_ = std::move(checker);
}

void FailoverManager::health_check_loop() {
    while (running_.load()) {
        perform_health_check();

        // 检查超时和恢复
        std::lock_guard<std::mutex> lock(nodes_mutex_);

        auto now = std::chrono::system_clock::now();

        for (auto it = node_health_.begin(); it != node_health_.end(); ) {
            auto& [id, health] = *it;

            // 检查超时
            if (health.state == NodeState::Unhealthy && is_node_timeout(health)) {
                update_node_state(id, NodeState::Failed, "Timeout");
            }

            // 检查是否可以恢复
            if (health.state == NodeState::Failed) {
                auto time_since_failure = std::chrono::duration_cast<std::chrono::seconds>(
                    now - health.last_check
                );

                if (time_since_failure >= config_.recovery_timeout) {
                    // 尝试恢复
                    if (check_node_health(id)) {
                        update_node_state(id, NodeState::Healthy, "Recovered after timeout");
                    }
                }
            }

            ++it;
        }

        std::this_thread::sleep_for(config_.health_check_interval);
    }
}

bool FailoverManager::check_node_health(const std::string& node_id) {
    auto node_it = nodes_.find(node_id);
    if (node_it == nodes_.end()) {
        return false;
    }

    // 如果有自定义的健康检查器，使用它
    if (health_checker_) {
        return health_checker_(node_it->second);
    }

    // 默认实现：总是返回 true（实际项目中应该实现真实的健康检查）
    // 例如：发送 ping、检查服务端口等
    return true;
}

void FailoverManager::update_node_state(const std::string& node_id, NodeState new_state, const std::string& reason) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto health_it = node_health_.find(node_id);
    if (health_it == node_health_.end()) {
        return;
    }

    NodeHealth& health = health_it->second;
    NodeState previous_state = health.state;

    if (new_state == previous_state) {
        return;
    }

    health.state = new_state;
    health.last_check = std::chrono::system_clock::now();

    if (new_state == NodeState::Unhealthy || new_state == NodeState::Failed) {
        health.consecutive_failures++;
        health.last_error = reason;

        if (health.consecutive_failures >= config_.max_consecutive_failures) {
            health.state = NodeState::Failed;
        }
    } else if (new_state == NodeState::Healthy) {
        health.consecutive_failures = 0;
        health.last_error.clear();
    }

    // 触发故障事件
    NodeFailureEvent event;
    event.node_id = node_id;
    event.previous_state = previous_state;
    event.new_state = health.state;
    event.timestamp = std::chrono::system_clock::now();
    event.reason = reason;

    notify_node_failure(event);
}

void FailoverManager::notify_node_failure(const NodeFailureEvent& event) {
    std::lock_guard<std::mutex> lock(callback_mutex_);

    if (failure_callback_) {
        failure_callback_(event);
    }

    // 打印日志
    std::cout << "[FailoverManager] Node " << event.node_id
              << " state changed: " << static_cast<int>(event.previous_state)
              << " -> " << static_cast<int>(event.new_state)
              << ", reason: " << event.reason << std::endl;
}

bool FailoverManager::is_node_timeout(const NodeHealth& health) const {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - health.last_check
    );

    return elapsed >= config_.failure_timeout;
}

END_NAMESPACE_CORE
