//
// Created by boil on 2026/1/20.
//

// ============================================================================
// connection.h - 网络连接类
// ============================================================================

#ifndef RENDU_NETWORK_CONNECTION_H
#define RENDU_NETWORK_CONNECTION_H

#include "common/network/types.h"
#include "common/asio/tcp_socket.h"
#include "common/asio/io_context.h"
#include "common/asio/post.h"
#include <string>
#include <memory>
#include <vector>
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Connection - 网络连接类
// ============================================================================

/**
 * @brief 网络连接类
 *
 * 封装网络连接的生命周期和 I/O 操作
 */
class RC_COMMON_API Connection
    : public std::enable_shared_from_this<Connection>
{
public:
    /**
     * @brief 构造函数（客户端使用）
     * @param ioContext I/O 上下文引用
     */
    explicit Connection(Asio::IoContext& ioContext);

    /**
     * @brief 构造函数（服务器端使用，已建立的连接）
     * @param ioContext I/O 上下文引用
     * @param socket 已连接的 socket
     */
    Connection(Asio::IoContext& ioContext, Asio::TcpSocket socket);

    /**
     * @brief 析构函数
     */
    ~Connection();

    // 禁用复制
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    // 支持移动
    Connection(Connection&&) noexcept = default;
    Connection& operator=(Connection&&) noexcept = default;

    /**
     * @brief 异步发送数据
     * @param data 数据指针
     * @param size 数据大小
     * @param onSend 发送完成回调
     */
    void asyncSend(const uint8* data, size_t size, OnSendCallback onSend);

    /**
     * @brief 异步发送字符串
     * @param str 字符串数据
     * @param onSend 发送完成回调
     */
    void asyncSend(const std::string& str, OnSendCallback onSend);

    /**
     * @brief 开始异步接收数据
     * @param onReceive 接收回调
     */
    void asyncReceive(OnReceiveCallback onReceive);

    /**
     * @brief 关闭连接
     */
    void close();

    /**
     * @brief 获取连接状态
     * @return 连接状态
     */
    [[nodiscard]] ConnectionState getState() const;

    /**
     * @brief 获取远程地址
     * @return 远程地址字符串
     */
    [[nodiscard]] std::string getRemoteAddress() const;

    /**
     * @brief 获取远程端口
     * @return 远程端口号
     */
    [[nodiscard]] uint16 getRemotePort() const;

    /**
     * @brief 设置连接回调
     * @param onClose 关闭回调
     */
    void setOnCloseCallback(OnCloseCallback onClose);

    /**
     * @brief 是否为服务器端连接
     * @return true 如果是服务器端连接
     */
    [[nodiscard]] bool isServerConnection() const { return m_isServer; }

    /**
     * @brief 获取连接 ID
     * @return 连接 ID
     */
    [[nodiscard]] uint64 getConnectionId() const { return m_connectionId; }

    /**
     * @brief 设置为已连接状态（供 Client 使用）
     */
    void markConnected() { setState(ConnectionState::Connected); }

    /**
     * @brief 获取内部 socket（供服务器使用）
     * @return Socket 引用
     */
    Asio::TcpSocket& getSocket() { return m_socket; }

private:
    /**
     * @brief 生成连接 ID
     * @return 连接 ID
     */
    static uint64 generateConnectionId();

    /**
     * @brief 设置连接状态
     * @param state 新状态
     */
    void setState(ConnectionState state);

    /**
     * @brief 触发关闭回调
     * @param ec 错误码
     */
    void notifyClose(const std::error_code& ec);

    /**
     * @brief 处理发送完成
     * @param bytesTransferred 发送字节数
     * @param ec 错误码
     */
    void handleSend(size_t bytesTransferred, const std::error_code& ec);

    /**
     * @brief 处理接收完成
     * @param bytesTransferred 接收字节数
     * @param ec 错误码
     */
    void handleReceive(size_t bytesTransferred, const std::error_code& ec);

private:
    Asio::IoContext& m_ioContext;          ///< I/O 上下文引用
    Asio::TcpSocket m_socket;               ///< TCP Socket
    bool m_isServer;                       ///< 是否为服务器端连接
    uint64 m_connectionId;                 ///< 连接唯一 ID
    std::atomic<ConnectionState> m_state{ConnectionState::Disconnected}; ///< 连接状态

    OnCloseCallback m_onClose;             ///< 关闭回调
    OnReceiveCallback m_onReceive;         ///< 接收回调
    OnSendCallback m_pendingSend;           ///< 待发送回调

    std::vector<uint8> m_recvBuffer;       ///< 接收缓冲区
    static constexpr size_t RECV_BUFFER_SIZE = 65536; ///< 接收缓冲区大小
    static std::atomic<uint64> s_nextConnectionId;   ///< 连接 ID 生成器
};

} // namespace Network

END_NAMESPACE_COMMON

#endif // RENDU_NETWORK_CONNECTION_H
