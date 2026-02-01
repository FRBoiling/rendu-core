#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>

#include "core/define.h"

BEGIN_NAMESPACE_CORE

/// 节点信息
struct NodeInfo {
    std::string node_id;                                 // 节点 ID
    std::string address;                                // 地址
    uint16_t port{0};                                   // 端口
    std::chrono::system_clock::time_point last_seen;    // 最后活跃时间
    std::chrono::system_clock::time_point first_seen;   // 首次发现时间

    bool is_expired(std::chrono::seconds timeout) const {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_seen);
        return elapsed > timeout;
    }
};

/// 节点发现事件
struct NodeEvent {
    enum class Type {
        Joined,   // 节点加入
        Left,     // 节点离开
        Updated   // 节点更新
    };

    Type type;
    NodeInfo node_info;
    std::chrono::system_clock::time_point timestamp;
};

/// 节点发现器
/// 用于自动发现和管理集群中的节点
class NodeDiscovery {
public:
    using NodeChangeCallback = std::function<void(const NodeEvent&)>;

    /// 配置
    struct Config {
        std::string node_id;                    // 本节点 ID（空则自动生成）
        std::string listen_address;             // 监听地址（默认 0.0.0.0）
        uint16_t listen_port{9001};            // 监听端口
        uint16_t broadcast_port{9001};          // 广播端口
        std::chrono::seconds announce_interval{5};       // 广播间隔
        std::chrono::seconds node_timeout{15};           // 节点超时时间
        std::chrono::seconds cleanup_interval{60};      // 清理间隔
    };

    explicit NodeDiscovery(const Config& config);
    ~NodeDiscovery();

    /// 启动节点发现
    void start();

    /// 停止节点发现
    void stop();

    /// 立即广播节点信息
    void broadcast();

    /// 获取所有活跃节点
    std::vector<NodeInfo> get_active_nodes() const;

    /// 获取节点数量
    size_t get_node_count() const;

    /// 获取本节点信息
    const NodeInfo& get_local_node() const;

    /// 获取指定节点信息
    std::optional<NodeInfo> get_node(const std::string& node_id) const;

    /// 注册节点变更回调
    void on_node_change(NodeChangeCallback callback);

    /// 注销节点变更回调
    void remove_node_change_callback();

    /// 手动添加节点（用于静态配置）
    void add_node(const NodeInfo& node);

    /// 移除节点
    void remove_node(const std::string& node_id);

    /// 清理过期节点
    void cleanup_expired_nodes();

private:
    /// 生成节点 ID
    std::string generate_node_id();

    /// 初始化 UDP socket
    bool init_socket();

    /// 监听广播消息
    void listen_loop();

    /// 广播节点信息
    void broadcast_loop();

    /// 检查节点活跃状态
    void check_liveness_loop();

    /// 处理接收到的公告
    void handle_announcement(const std::string& data, const std::string& from_address);

    /// 构造公告消息
    std::string build_announcement();

    /// 解析公告消息
    std::optional<NodeInfo> parse_announcement(const std::string& data);

    /// 触发节点变更事件
    void notify_node_change(const NodeEvent& event);

private:
    Config config_;
    NodeInfo local_node_;
    std::unordered_map<std::string, NodeInfo> nodes_;
    NodeChangeCallback node_change_callback_;

    // 定时器和线程
    int socket_fd_{-1};
    std::unique_ptr<std::thread> listen_thread_;
    std::unique_ptr<std::thread> broadcast_thread_;
    std::unique_ptr<std::thread> liveness_thread_;
    std::atomic<bool> running_{false};

    mutable std::mutex nodes_mutex_;
    std::mutex callback_mutex_;
};

END_NAMESPACE_CORE
