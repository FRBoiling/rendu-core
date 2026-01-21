//
// Created by boil on 2026/1/20.
//

// ============================================================================
// server.h - 网络服务器
// ============================================================================

#ifndef RENDU_NETWORK_SERVER_H
#define RENDU_NETWORK_SERVER_H

#include "common/network/types.h"
#include "common/network/connection.h"
#include "common/network/manager.h"
#include "common/asio/tcp_socket.h"
#include "common/asio/io_context.h"
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Server - TCP 服务器
// ============================================================================

/**
 * @brief TCP 服务器类
 *
 * 提供异步 TCP 服务器监听和连接管理功能
 */
class RC_COMMON_API Server
{
public:
    /**
     * @brief 构造函数
     * @param ioContext I/O 上下文引用
     */
    explicit Server(Asio::IoContext& ioContext);

    /**
     * @brief 析构函数
     */
    ~Server();

    // 禁用复制
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // 支持移动
    Server(Server&&) noexcept = default;
    Server& operator=(Server&&) noexcept = default;

    /**
     * @brief 启动服务器监听
     * @param host 监听地址
     * @param port 监听端口
     * @param onAccept 接受连接回调
     * @return true 如果成功启动
     */
    bool start(
        const std::string& host,
        uint16 port,
        OnAcceptCallback onAccept);

    /**
     * @brief 停止服务器
     */
    void stop();

    /**
     * @brief 是否正在运行
     * @return true 如果正在运行
     */
    [[nodiscard]] bool isRunning() const;

    /**
     * @brief 获取监听地址
     * @return 监听地址字符串
     */
    [[nodiscard]] std::string getListenAddress() const;

    /**
     * @brief 获取监听端口
     * @return 监听端口号
     */
    [[nodiscard]] uint16 getListenPort() const;

    /**
     * @brief 获取所有活动连接
     * @return 连接列表
     */
    [[nodiscard]] std::vector<std::shared_ptr<Connection>> getConnections() const;

    /**
     * @brief 获取连接数
     * @return 连接数
     */
    [[nodiscard]] size_t getConnectionCount() const;

private:
    /**
     * @brief 开始异步接受连接
     */
    void startAccept();

    /**
     * @brief 处理新的连接
     * @param ec 错误码
     */
    void handleAccept(const std::error_code& ec);

    /**
     * @brief 移除连接
     * @param connection 要移除的连接
     */
    void removeConnection(std::shared_ptr<Connection> connection);

private:
    Asio::IoContext& m_ioContext;         ///< I/O 上下文引用
    std::unique_ptr<Asio::TcpAcceptor> m_acceptor; ///< TCP 接受器
    std::unique_ptr<Asio::TcpSocket> m_acceptSocket; ///< 接受连接的 socket
    std::string m_listenAddress;          ///< 监听地址
    uint16 m_listenPort = 0;             ///< 监听端口
    OnAcceptCallback m_onAccept;          ///< 接受连接回调
    std::atomic<bool> m_running{false};  ///< 是否正在运行
    mutable std::mutex m_mutex;          ///< 互斥锁
    std::vector<std::shared_ptr<Connection>> m_connections; ///< 连接列表
};

} // namespace Network

END_NAMESPACE_COMMON

#endif // RENDU_NETWORK_SERVER_H
