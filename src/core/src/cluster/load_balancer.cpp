#include "core/cluster/load_balancer.h"

#include <random>
#include <algorithm>
#include <numeric>

BEGIN_NAMESPACE_CORE

LoadBalancer::LoadBalancer(LoadBalanceStrategy strategy)
    : strategy_(strategy)
    , rng_(std::random_device{}()) {
}

void LoadBalancer::set_strategy(LoadBalanceStrategy strategy) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    strategy_ = strategy;
    // 切换策略时重置轮询索引
    if (strategy == LoadBalanceStrategy::RoundRobin) {
        round_robin_index_.store(0, std::memory_order_relaxed);
    }
}

void LoadBalancer::add_node(const LoadBalancedNode& node) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    nodes_[node.node_id] = node;
    node_order_.push_back(node.node_id);
}

void LoadBalancer::remove_node(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        nodes_.erase(it);
    }

    // 从有序列表中移除
    node_order_.erase(
        std::remove(node_order_.begin(), node_order_.end(), node_id),
        node_order_.end()
    );
}

void LoadBalancer::update_node_weight(const std::string& node_id, int weight) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        it->second.weight = weight;
    }
}

void LoadBalancer::increment_connections(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        it->second.connections++;
    }
}

void LoadBalancer::decrement_connections(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    auto it = nodes_.find(node_id);
    if (it != nodes_.end() && it->second.connections > 0) {
        it->second.connections--;
    }
}

std::optional<LoadBalancedNode> LoadBalancer::select() {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    if (nodes_.empty()) {
        return std::nullopt;
    }

    switch (strategy_) {
        case LoadBalanceStrategy::RoundRobin:
            return select_round_robin();
        case LoadBalanceStrategy::LeastConnections:
            return select_least_connections();
        case LoadBalanceStrategy::Weighted:
            return select_weighted();
        case LoadBalanceStrategy::Random:
            return select_random();
        default:
            return select_round_robin();
    }
}

std::vector<LoadBalancedNode> LoadBalancer::get_all_nodes() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);

    std::vector<LoadBalancedNode> result;
    result.reserve(nodes_.size());

    for (const auto& [id, node] : nodes_) {
        result.push_back(node);
    }

    return result;
}

size_t LoadBalancer::get_node_count() const {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    return nodes_.size();
}

void LoadBalancer::clear() {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    nodes_.clear();
    node_order_.clear();
    round_robin_index_.store(0, std::memory_order_relaxed);
}

std::optional<LoadBalancedNode> LoadBalancer::select_round_robin() {
    if (node_order_.empty()) {
        return std::nullopt;
    }

    size_t index = round_robin_index_.fetch_add(1, std::memory_order_relaxed) % node_order_.size();
    const std::string& node_id = node_order_[index];

    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        return it->second;
    }

    return std::nullopt;
}

std::optional<LoadBalancedNode> LoadBalancer::select_least_connections() {
    if (nodes_.empty()) {
        return std::nullopt;
    }

    auto it = std::min_element(
        nodes_.begin(),
        nodes_.end(),
        [](const auto& a, const auto& b) {
            return a.second.connections < b.second.connections;
        }
    );

    return it->second;
}

std::optional<LoadBalancedNode> LoadBalancer::select_weighted() {
    if (nodes_.empty()) {
        return std::nullopt;
    }

    // 计算总权重
    int total_weight = 0;
    std::vector<std::pair<std::string, int>> weighted_nodes;

    for (const auto& [id, node] : nodes_) {
        total_weight += node.weight;
        weighted_nodes.emplace_back(id, total_weight);
    }

    // 随机选择
    std::lock_guard<std::mutex> rng_lock(rng_mutex_);
    std::uniform_int_distribution<int> dist(1, total_weight);
    int random_value = dist(rng_);

    // 找到对应的节点
    for (const auto& [id, weight] : weighted_nodes) {
        if (random_value <= weight) {
            auto it = nodes_.find(id);
            if (it != nodes_.end()) {
                return it->second;
            }
        }
    }

    return nodes_.begin()->second;
}

std::optional<LoadBalancedNode> LoadBalancer::select_random() {
    if (nodes_.empty()) {
        return std::nullopt;
    }

    std::lock_guard<std::mutex> rng_lock(rng_mutex_);
    std::uniform_int_distribution<size_t> dist(0, nodes_.size() - 1);
    size_t index = dist(rng_);

    auto it = nodes_.begin();
    std::advance(it, index);

    return it->second;
}

uint64_t LoadBalancer::compute_hash(const std::string& node_id) {
    // 简单的哈希函数，用于一致性哈希（预留）
    // 实际实现可以使用更好的哈希算法
    uint64_t hash = 0;
    for (char c : node_id) {
        hash = hash * 31 + c;
    }
    return hash;
}

END_NAMESPACE_CORE
