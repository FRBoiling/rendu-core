#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <mutex>
#include <atomic>
#include <random>

#include "core/define.h"

BEGIN_NAMESPACE_CORE

/// 节点信息（用于负载均衡）
struct LoadBalancedNode {
    std::string node_id;                                 // 节点 ID
    std::string address;                                // 地址
    uint16_t port{0};                                   // 端口
    int weight{1};                                       // 权重（用于加权策略）
    int connections{0};                                  // 当前连接数（用于最少连接策略）

    // 用于一致性哈希
    uint64_t hash{0};
};

/// 负载均衡策略
enum class LoadBalanceStrategy {
    RoundRobin,        // 轮询
    LeastConnections,  // 最少连接
    Weighted,          // 加权随机
    Random              // 随机
};

/// 负载均衡器
class LoadBalancer {
public:
    explicit LoadBalancer(LoadBalanceStrategy strategy = LoadBalanceStrategy::RoundRobin);

    /// 设置负载均衡策略
    void set_strategy(LoadBalanceStrategy strategy);

    /// 添加节点
    void add_node(const LoadBalancedNode& node);

    /// 移除节点
    void remove_node(const std::string& node_id);

    /// 更新节点权重
    void update_node_weight(const std::string& node_id, int weight);

    /// 增加连接数
    void increment_connections(const std::string& node_id);

    /// 减少连接数
    void decrement_connections(const std::string& node_id);

    /// 选择节点（不带任何参数）
    std::optional<LoadBalancedNode> select();

    /// 获取所有节点
    std::vector<LoadBalancedNode> get_all_nodes() const;

    /// 获取活跃节点数量
    size_t get_node_count() const;

    /// 清空所有节点
    void clear();

private:
    /// 轮询策略
    std::optional<LoadBalancedNode> select_round_robin();

    /// 最少连接策略
    std::optional<LoadBalancedNode> select_least_connections();

    /// 加权随机策略
    std::optional<LoadBalancedNode> select_weighted();

    /// 随机策略
    std::optional<LoadBalancedNode> select_random();

    /// 计算节点哈希（用于一致性哈希，预留）
    uint64_t compute_hash(const std::string& node_id);

private:
    LoadBalanceStrategy strategy_;
    std::unordered_map<std::string, LoadBalancedNode> nodes_;
    std::vector<std::string> node_order_;  // 用于轮询的有序列表

    // 轮询索引
    std::atomic<size_t> round_robin_index_{0};

    // 随机数生成器
    mutable std::mt19937 rng_;
    mutable std::mutex rng_mutex_;

    mutable std::mutex nodes_mutex_;
};

END_NAMESPACE_CORE
