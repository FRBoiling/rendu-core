//
// Created by boil on 2026/1/20.
//

// ============================================================================
// client.h - 网络客户端
// ============================================================================

#ifndef RENDU_NETWORK_CLIENT_H
#define RENDU_NETWORK_CLIENT_H

#include "common/network/types.h"
#include "common/network/connection.h"
#include "common/network/manager.h"
#include "common/asio/tcp_socket.h"
#include "common/asio/io_context.h"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Client - TCP 客户端
// ============================================================================

/**
 * @brief TCP 客户端类
 *
 * 提供异步 TCP 客户端连接功能
 */
class RC_COMMON_API Client
{
public:
    /**
     * @brief 构造函数
     * @param ioContext I/O 上下文引用
     */
    explicit Client(Asio::IoContext& ioContext);

    /**
     * @brief 析构函数
     */
    ~Client();

    // 禁用复制
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // 支持移动
    Client(Client&&) noexcept = default;
    Client& operator=(Client&&) noexcept = default;

    /**
     * @brief 异步连接到服务器
     * @param host 主机地址
     * @param port 端口号
     * @param onConnect 连接回调
     * @return 连接对象指针
     */
    std::shared_ptr<Connection> asyncConnect(
        const std::string& host,
        uint16 port,
        OnConnectCallback onConnect);

    /**
     * @brief 获取当前连接
     * @return 连接指针（如果未连接则返回 nullptr）
     */
    [[nodiscard]] std::shared_ptr<Connection> getConnection() const;

    /**
     * @brief 断开连接
     */
    void disconnect();

    /**
     * @brief 是否已连接
     * @return true 如果已连接
     */
    [[nodiscard]] bool isConnected() const;

private:
    Asio::IoContext& m_ioContext;           ///< I/O 上下文引用
    std::unique_ptr<Asio::TcpSocket> m_socket; ///< Socket
    std::shared_ptr<Connection> m_connection; ///< 当前连接
    mutable std::mutex m_mutex;            ///< 互斥锁
};

} // namespace Network

END_NAMESPACE_COMMON

#endif // RENDU_NETWORK_CLIENT_H
