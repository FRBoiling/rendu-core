#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

#include "core/cluster/load_balancer.h"
#include "core/define.h"

BEGIN_NAMESPACE_CORE

/// 节点状态
enum class NodeState {
    Healthy,    // 健康
    Unhealthy,  // 不健康
    Failed       // 失败
};

/// 节点健康状态
struct NodeHealth {
    std::string node_id;
    NodeState state;
    std::chrono::system_clock::time_point last_check;
    int consecutive_failures;  // 连续失败次数
    std::string last_error;
};

/// 节点故障事件
struct NodeFailureEvent {
    std::string node_id;
    NodeState previous_state;
    NodeState new_state;
    std::chrono::system_clock::time_point timestamp;
    std::string reason;
};

/// 故障转移管理器
class FailoverManager {
public:
    using NodeFailureCallback = std::function<void(const NodeFailureEvent&)>;

    /// 配置
    struct Config {
        std::chrono::milliseconds health_check_interval{5000};  // 健康检查间隔
        std::chrono::milliseconds failure_timeout{15000};        // 失败超时时间
        std::chrono::milliseconds recovery_timeout{10000};       // 恢复超时时间
        int max_consecutive_failures{3};                        // 最大连续失败次数
        bool auto_failover{true};                                // 自动故障转移
    };

    explicit FailoverManager(const Config& config);

    ~FailoverManager();

    /// 启动故障转移管理
    void start();

    /// 停止故障转移管理
    void stop();

    /// 添加节点
    void add_node(const LoadBalancedNode& node);

    /// 移除节点
    void remove_node(const std::string& node_id);

    /// 标记节点失败
    void mark_node_failed(const std::string& node_id, const std::string& reason);

    /// 标记节点恢复
    void mark_node_recovered(const std::string& node_id);

    /// 获取节点健康状态
    std::optional<NodeHealth> get_node_health(const std::string& node_id) const;

    /// 获取所有健康节点
    std::vector<LoadBalancedNode> get_healthy_nodes() const;

    /// 获取所有失败节点
    std::vector<NodeHealth> get_failed_nodes() const;

    /// 获取节点数量
    size_t get_node_count() const;

    /// 获取健康节点数量
    size_t get_healthy_node_count() const;

    /// 注册节点故障回调
    void on_node_failure(NodeFailureCallback callback);

    /// 手动触发健康检查
    void perform_health_check();

    /// 执行健康检查（可自定义检查函数）
    void set_health_checker(std::function<bool(const LoadBalancedNode&)> checker);

private:
    /// 健康检查循环
    void health_check_loop();

    /// 检查单个节点健康
    bool check_node_health(const std::string& node_id);

    /// 更新节点状态
    void update_node_state(const std::string& node_id, NodeState new_state, const std::string& reason);

    /// 触发节点失败事件
    void notify_node_failure(const NodeFailureEvent& event);

    /// 节点是否超时
    bool is_node_timeout(const NodeHealth& health) const;

private:
    Config config_;
    std::unordered_map<std::string, LoadBalancedNode> nodes_;
    std::unordered_map<std::string, NodeHealth> node_health_;

    NodeFailureCallback failure_callback_;
    std::function<bool(const LoadBalancedNode&)> health_checker_;

    // 线程
    std::unique_ptr<std::thread> health_check_thread_;
    std::atomic<bool> running_{false};

    mutable std::mutex nodes_mutex_;
    mutable std::mutex callback_mutex_;
};

END_NAMESPACE_CORE
