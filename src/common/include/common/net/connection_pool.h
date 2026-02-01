#pragma once

#include "common/define.h"
#include "common/net/channel.h"
#include "common/io/io_context.h"
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <future>
#include <vector>

BEGIN_NAMESPACE_COMMON
namespace net {

/**
 * @brief 连接池配置
 */
struct ConnectionPoolConfig {
    size_t max_connections = 100;                           // 最大连接数
    std::chrono::seconds idle_timeout{300};                 // 空闲超时时间
    std::chrono::seconds connect_timeout{10};               // 连接超时时间
    std::shared_ptr<Codec> codec;                           // 编解码器
};

/**
 * @brief 连接池
 *
 * 复用 TCP 连接,减少频繁创建/销毁连接的开销
 * 支持连接超时、空闲连接清理等功能
 */
class ConnectionPool {
public:
    /**
     * @brief 构造函数
     * @param io IoContext 引用
     * @param host 主机名或 IP
     * @param port 端口号
     * @param config 配置
     */
    ConnectionPool(
        io::IoContext& io,
        const std::string& host,
        uint16_t port,
        const ConnectionPoolConfig& config = ConnectionPoolConfig{}
    );

    ~ConnectionPool();

    // 禁止拷贝和移动
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;
    ConnectionPool(ConnectionPool&&) = delete;
    ConnectionPool& operator=(ConnectionPool&&) = delete;

    /**
     * @brief 获取连接 (异步)
     * @return Future 包含 ChannelPtr
     *
     * 如果有空闲连接,立即返回;否则创建新连接
     */
    std::future<std::shared_ptr<Channel>> acquire();

    /**
     * @brief 归还连接
     * @param channel 连接
     *
     * 连接会被放回空闲列表供后续复用
     * 如果连接已关闭,则不会被放回
     */
    void release(std::shared_ptr<Channel> channel);

    /**
     * @brief 清理空闲连接
     *
     * 清理超时的空闲连接
     */
    void cleanup_idle_connections();

    /**
     * @brief 关闭所有连接
     */
    void close_all();

    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t idle_count;       // 空闲连接数
        size_t active_count;     // 活跃连接数
        size_t total_count;      // 总连接数
    };

    Stats get_stats() const;

private:
    /**
     * @brief 创建新连接
     * @return ChannelPtr
     */
    std::shared_ptr<Channel> create_connection();

    /**
     * @brief 检查连接是否可用
     * @param channel 连接
     * @return 是否可用
     */
    bool is_connection_valid(const std::shared_ptr<Channel>& channel) const;

    io::IoContext& io_;
    std::string host_;
    uint16_t port_;
    ConnectionPoolConfig config_;

    // 空闲连接队列
    std::queue<std::shared_ptr<Channel>> idle_connections_;
    std::queue<std::shared_ptr<std::promise<std::shared_ptr<Channel>>>> pending_requests_;

    // 互斥和条件变量
    mutable std::mutex mutex_;
    std::condition_variable cv_;

    // 统计
    std::atomic<size_t> active_count_{0};
    std::atomic<size_t> total_created_{0};
};

} // namespace net
END_NAMESPACE_COMMON
