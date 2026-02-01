#pragma once

#include "common/define.h"
#include "common/io/io_context.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/system/error_code.hpp>

BEGIN_NAMESPACE_COMMON
namespace net {

/**
 * @brief Socket 错误码枚举
 */
enum class SocketError {
    Success = 0,
    ConnectionRefused,      // 连接被拒绝
    ConnectionTimeout,     // 连接超时
    ConnectionReset,       // 连接被重置
    Disconnected,         // 已断开
    SendFailed,          // 发送失败
    ReceiveFailed,       // 接收失败
    InvalidEndpoint,      // 无效端点
    AlreadyConnected,     // 已连接
    NotConnected,        // 未连接
    BufferOverflow,      // 缓冲区溢出
    Unknown
};

/**
 * @brief Socket 回调类型
 */
using SocketCallback = std::function<void(const boost::system::error_code& ec)>;
using SendCallback = std::function<void(const boost::system::error_code& ec, size_t bytes_sent)>;
using ReceiveCallback = std::function<void(const boost::system::error_code& ec, std::vector<byte> data)>;

/**
 * @brief TCP Socket 封装类
 * 
 * 提供异步的 TCP 连接、发送、接收功能
 * 线程安全：所有操作都通过 io_context 调度
 */
class TcpSocket {
public:
    /**
     * @brief 构造函数
     * @param io IoContext 引用
     */
    explicit TcpSocket(io::IoContext& io);
    
    ~TcpSocket();

    // 禁止拷贝和移动
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;
    TcpSocket(TcpSocket&&) = delete;
    TcpSocket& operator=(TcpSocket&&) = delete;

    /**
     * @brief 异步连接到指定主机和端口
     * @param host 主机名或 IP 地址
     * @param port 端口号
     * @param callback 连接回调
     */
    void async_connect(const std::string& host, uint16_t port, SocketCallback callback);

    /**
     * @brief 异步连接到指定端点
     * @param endpoint TCP 端点
     * @param callback 连接回调
     */
    void async_connect(const boost::asio::ip::tcp::endpoint& endpoint, SocketCallback callback);

    /**
     * @brief 异步发送数据
     * @param data 要发送的数据
     * @param callback 发送回调
     *
     * 线程安全：可以从任意线程调用
     */
    void async_send(const std::vector<byte>& data, SendCallback callback);

    /**
     * @brief 零拷贝异步发送数据
     * @param data 数据指针
     * @param size 数据大小
     * @param callback 发送回调
     *
     * 线程安全：可以从任意线程调用
     * @warning 调用者需确保数据在发送完成前保持有效
     */
    void async_send_zero_copy(const void* data, size_t size, SendCallback callback);

    /**
     * @brief 异步接收指定长度的数据
     * @param size 要接收的字节数
     * @param callback 接收回调
     *
     * 线程安全：可以从任意线程调用
     */
    void async_receive(size_t size, ReceiveCallback callback);

    /**
     * @brief 关闭 Socket
     */
    void close();

    /**
     * @brief 检查是否已连接
     * @return 是否连接
     */
    bool is_connected() const;

    /**
     * @brief 检查 socket 是否打开
     * @return 是否打开
     */
    bool is_open() const;

    /**
     * @brief 获取底层 socket
     * @return boost::asio::ip::tcp::socket 引用
     */
    boost::asio::ip::tcp::socket& native_socket();

    /**
     * @brief 获取底层 socket（const 版本）
     * @return const boost::asio::ip::tcp::socket 引用
     */
    const boost::asio::ip::tcp::socket& native_socket() const;

    /**
     * @brief 设置底层 socket（用于接受连接）
     * @param socket 要设置的 socket
     */
    void set_socket(boost::asio::ip::tcp::socket&& socket);

    /**
     * @brief 获取本地端点
     * @return 本地端点
     */
    boost::asio::ip::tcp::endpoint local_endpoint() const;

    /**
     * @brief 获取远程端点
     * @return 远程端点
     */
    boost::asio::ip::tcp::endpoint remote_endpoint() const;

private:
    io::IoContext& io_;
    boost::asio::ip::tcp::socket socket_;
    std::atomic<bool> connected_;
};

/**
 * @brief TCP 接受器（服务器）
 *
 * 用于监听端口并接受新连接
 */
class TcpAcceptor {
public:
    /**
     * @brief 构造函数
     * @param io IoContext 引用
     * @param port 监听端口号
     */
    explicit TcpAcceptor(io::IoContext& io, uint16_t port);
    
    ~TcpAcceptor();

    // 禁止拷贝和移动
    TcpAcceptor(const TcpAcceptor&) = delete;
    TcpAcceptor& operator=(const TcpAcceptor&) = delete;
    TcpAcceptor(TcpAcceptor&&) = delete;
    TcpAcceptor& operator=(TcpAcceptor&&) = delete;

    /**
     * @brief 接受回调类型
     */
    using AcceptCallback = std::function<void(std::shared_ptr<TcpSocket>, const boost::system::error_code&)>;

    /**
     * @brief 异步接受连接
     * @param callback 接受回调
     */
    void async_accept(AcceptCallback callback);

    /**
     * @brief 关闭接受器
     */
    void close();

    /**
     * @brief 检查是否正在监听
     * @return 是否监听中
     */
    bool is_listening() const;

    /**
     * @brief 获取监听地址
     * @return 监听端点
     */
    boost::asio::ip::tcp::endpoint local_endpoint() const;

private:
    io::IoContext& io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::atomic<bool> listening_;
};

/**
 * @brief UDP Socket 封装类
 *
 * 提供异步的 UDP 发送、接收功能（无连接协议）
 * 线程安全：所有操作都通过 io_context 调度
 */
class UdpSocket {
public:
    /**
     * @brief 构造函数
     * @param io IoContext 引用
     * @param port 本地绑定端口（0 表示自动分配）
     */
    explicit UdpSocket(io::IoContext& io, uint16_t port = 0);

    ~UdpSocket();

    // 禁止拷贝和移动
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;
    UdpSocket(UdpSocket&&) = delete;
    UdpSocket& operator=(UdpSocket&&) = delete;

    /**
     * @brief 异步发送数据到指定端点
     * @param data 要发送的数据
     * @param endpoint 目标端点
     * @param callback 发送回调
     */
    void async_send_to(const std::vector<byte>& data,
                      const boost::asio::ip::udp::endpoint& endpoint,
                      SendCallback callback);

    /**
     * @brief 异步接收数据
     * @param size 接收缓冲区大小
     * @param callback 接收回调
     */
    void async_receive_from(size_t size, ReceiveCallback callback);

    /**
     * @brief 绑定到指定端口
     * @param port 端口号
     * @param multicast_addr 多播地址（可选）
     */
    void bind(uint16_t port, const std::string& multicast_addr = "");

    /**
     * @brief 加入多播组
     * @param multicast_addr 多播地址
     */
    void join_multicast(const std::string& multicast_addr);

    /**
     * @brief 离开多播组
     * @param multicast_addr 多播地址
     */
    void leave_multicast(const std::string& multicast_addr);

    /**
     * @brief 设置广播选项
     * @param enable 是否启用广播
     */
    void set_broadcast(bool enable);

    /**
     * @brief 关闭 Socket
     */
    void close();

    /**
     * @brief 检查 socket 是否打开
     * @return 是否打开
     */
    bool is_open() const;

    /**
     * @brief 获取本地端点
     * @return 本地端点
     */
    boost::asio::ip::udp::endpoint local_endpoint() const;

    /**
     * @brief 获取底层 socket
     * @return boost::asio::ip::udp::socket 引用
     */
    boost::asio::ip::udp::socket& native_socket();

    /**
     * @brief 获取底层 socket（const 版本）
     * @return const boost::asio::ip::udp::socket 引用
     */
    const boost::asio::ip::udp::socket& native_socket() const;

private:
    io::IoContext& io_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
};

// ============================================================================
// KCP Socket 占位声明（需要集成 KCP 库后实现）
// ============================================================================

// KCP Socket 将在未来版本中实现
// 需要引入 KCP 库 (https://github.com/skywind3000/kcp)

} // namespace net
END_NAMESPACE_COMMON
