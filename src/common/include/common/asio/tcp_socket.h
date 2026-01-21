//
// Created by boil on 2026/1/21.
//

// ============================================================================
// tcp_socket.h - TCP Socket 封装
// ============================================================================

#ifndef RENDU_TCP_SOCKET_H
#define RENDU_TCP_SOCKET_H

#include "common/asio/io_context.h"
#include "common/define.h"
#include <string>
#include <vector>
#include <memory>
#include <system_error>

BEGIN_NAMESPACE_COMMON

namespace Asio
{

// ============================================================================
// TcpSocket - TCP Socket 封装
// ============================================================================

// 前向声明
class TcpAcceptor;

class TcpSocket
{
public:
    /**
     * @brief 构造函数
     * @param ioContext I/O 上下文
     */
    explicit TcpSocket(IoContext& ioContext);

    /**
     * @brief 析构函数
     */
    ~TcpSocket();

    // 禁用复制
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    // 支持移动
    TcpSocket(TcpSocket&&) noexcept;
    TcpSocket& operator=(TcpSocket&&) noexcept;

    /**
     * @brief 异步连接到远程端点
     * @param host 主机地址
     * @param port 端口号
     * @param onConnect 连接完成回调
     */
    void asyncConnect(
        const std::string& host,
        uint16 port,
        std::function<void(const std::error_code&)> onConnect);

    /**
     * @brief 异步发送数据
     * @param data 数据指针
     * @param size 数据大小
     * @param onSend 发送完成回调
     */
    void asyncSend(
        const uint8* data,
        size_t size,
        std::function<void(size_t, const std::error_code&)> onSend);

    /**
     * @brief 异步发送数据
     * @param data 数据向量
     * @param onSend 发送完成回调
     */
    void asyncSend(
        const std::vector<uint8>& data,
        std::function<void(size_t, const std::error_code&)> onSend);

    /**
     * @brief 异步接收数据
     * @param buffer 接收缓冲区
     * @param maxSize 最大接收大小
     * @param onReceive 接收完成回调
     */
    void asyncReceive(
        uint8* buffer,
        size_t maxSize,
        std::function<void(size_t, const std::error_code&)> onReceive);

    /**
     * @brief 异步接收指定字节数
     * @param buffer 接收缓冲区
     * @param size 要接收的字节数
     * @param onReceive 接收完成回调
     */
    void asyncReceiveExact(
        uint8* buffer,
        size_t size,
        std::function<void(const std::error_code&)> onReceive);

    /**
     * @brief 关闭 socket
     */
    void close();

    /**
     * @brief 关闭写入端
     */
    void shutdownWrite();

    /**
     * @brief 获取远程地址
     * @return 远程地址
     */
    [[nodiscard]] std::string getRemoteAddress() const;

    /**
     * @brief 获取远程端口
     * @return 远程端口
     */
    [[nodiscard]] uint16 getRemotePort() const;

    /**
     * @brief 获取本地地址
     * @return 本地地址
     */
    [[nodiscard]] std::string getLocalAddress() const;

    /**
     * @brief 获取本地端口
     * @return 本地端口
     */
    [[nodiscard]] uint16 getLocalPort() const;

    /**
     * @brief 是否已打开
     * @return true 如果已打开
     */
    [[nodiscard]] bool isOpen() const;

    /**
     * @brief 取消所有异步操作
     */
    void cancel();


private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;

    friend class TcpAcceptor;
};

// ============================================================================
// TcpAcceptor - TCP 接受器封装
// ============================================================================

class TcpAcceptor
{
public:
    /**
     * @brief 构造函数
     * @param ioContext I/O 上下文
     */
    explicit TcpAcceptor(IoContext& ioContext);

    /**
     * @brief 析构函数
     */
    ~TcpAcceptor();

    // 禁用复制
    TcpAcceptor(const TcpAcceptor&) = delete;
    TcpAcceptor& operator=(const TcpAcceptor&) = delete;

    // 支持移动
    TcpAcceptor(TcpAcceptor&&) noexcept;
    TcpAcceptor& operator=(TcpAcceptor&&) noexcept;

    /**
     * @brief 绑定并开始监听
     * @param address 监听地址
     * @param port 监听端口
     * @param reuseAddress 是否重用地址
     * @return 成功返回 true
     */
    bool bind(const std::string& address, uint16 port, bool reuseAddress = true);

    /**
     * @brief 异步接受连接
     * @param socket 要接受连接的 socket
     * @param onAccept 接受完成回调
     */
    void asyncAccept(
        TcpSocket& socket,
        std::function<void(const std::error_code&)> onAccept);

    /**
     * @brief 关闭接受器
     */
    void close();

    /**
     * @brief 是否已打开
     * @return true 如果已打开
     */
    [[nodiscard]] bool isOpen() const;

    /**
     * @brief 获取监听地址
     * @return 监听地址
     */
    [[nodiscard]] std::string getListenAddress() const;

    /**
     * @brief 获取监听端口
     * @return 监听端口
     */
    [[nodiscard]] uint16 getListenPort() const;


private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_TCP_SOCKET_H
