#include "common/net/socket.h"
#include "common/log/logger.h"
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/connect.hpp>
#include <sstream>

BEGIN_NAMESPACE_COMMON
namespace net {

// ============================================================================
// TcpSocket 实现
// ============================================================================

TcpSocket::TcpSocket(io::IoContext& io)
    : io_(io)
    , socket_(io.native())
    , connected_(false)
{
    // 打开 socket（IPv4 协议）
    boost::system::error_code ec;
    socket_.open(boost::asio::ip::tcp::v4(), ec);
    if (ec) {
        RENDU_LOG_WARN("Failed to open socket: {}", ec.message());
    }
}

TcpSocket::~TcpSocket() {
    close();
}

void TcpSocket::async_connect(const std::string& host, uint16_t port, SocketCallback callback) {
    // 使用 resolver 解析域名
    auto resolver = std::make_shared<boost::asio::ip::tcp::resolver>(io_.native());

    resolver->async_resolve(
        host, std::to_string(port),
        [this, callback, resolver](const boost::system::error_code& ec,
                                  boost::asio::ip::tcp::resolver::results_type results) {
            if (ec) {
                callback(ec);
                return;
            }

            // 连接到第一个解析的端点（手动遍历）
            if (results.empty()) {
                boost::system::error_code ec(boost::system::errc::host_unreachable,
                                          boost::system::system_category());
                callback(ec);
                return;
            }

            // 使用第一个端点
            async_connect(*results.begin(), callback);
        }
    );
}

void TcpSocket::async_connect(const boost::asio::ip::tcp::endpoint& endpoint, SocketCallback callback) {
    socket_.async_connect(endpoint,
        [this, callback](const boost::system::error_code& ec) {
            if (!ec) {
                connected_.store(true);
            }
            callback(ec);
        }
    );
}

void TcpSocket::async_send(const std::vector<byte>& data, SendCallback callback) {
    // 检查连接状态
    if (!connected_.load()) {
        boost::system::error_code ec(boost::system::errc::not_connected,
                                  boost::system::system_category());
        callback(ec, 0);
        return;
    }
    
    // 异步发送
    boost::asio::async_write(
        socket_, boost::asio::buffer(data),
        [callback](const boost::system::error_code& ec, size_t bytes_sent) {
            callback(ec, bytes_sent);
        }
    );
}

void TcpSocket::async_receive(size_t size, ReceiveCallback callback) {
    // 检查连接状态
    if (!connected_.load()) {
        boost::system::error_code ec(boost::system::errc::not_connected,
                                  boost::system::system_category());
        callback(ec, {});
        return;
    }
    
    // 准备接收缓冲区
    auto buffer = std::make_shared<std::vector<byte>>(size);
    
    // 异步接收
    boost::asio::async_read(
        socket_, boost::asio::buffer(*buffer),
        [callback, buffer](const boost::system::error_code& ec, size_t bytes_received) {
            if (ec || bytes_received == 0) {
                // 连接关闭或出错
                callback(ec, {});
            } else {
                // 调整缓冲区大小到实际接收的字节数
                buffer->resize(bytes_received);
                callback(ec, std::move(*buffer));
            }
        }
    );
}

void TcpSocket::close() {
    boost::system::error_code ec;
    socket_.close(ec);

    if (ec) {
        // 记录关闭错误
        RENDU_LOG_WARN("Socket close error: {}", ec.message());
    }

    connected_.store(false);
}

bool TcpSocket::is_connected() const {
    return connected_.load();
}

bool TcpSocket::is_open() const {
    return socket_.is_open();
}

boost::asio::ip::tcp::socket& TcpSocket::native_socket() {
    return socket_;
}

const boost::asio::ip::tcp::socket& TcpSocket::native_socket() const {
    return socket_;
}

void TcpSocket::set_socket(boost::asio::ip::tcp::socket&& socket) {
    socket_ = std::move(socket);
    connected_.store(true);
}

boost::asio::ip::tcp::endpoint TcpSocket::local_endpoint() const {
    boost::system::error_code ec;
    auto endpoint = socket_.local_endpoint(ec);

    if (ec) {
        RENDU_LOG_ERROR("Get local endpoint failed: {}", ec.message());
    }

    return endpoint;
}

boost::asio::ip::tcp::endpoint TcpSocket::remote_endpoint() const {
    boost::system::error_code ec;
    auto endpoint = socket_.remote_endpoint(ec);

    if (ec) {
        RENDU_LOG_ERROR("Get remote endpoint failed");
    }

    return endpoint;
}

// ============================================================================
// TcpAcceptor 实现
// ============================================================================

TcpAcceptor::TcpAcceptor(io::IoContext& io, uint16_t port)
    : io_(io)
    , acceptor_(io.native())
    , listening_(false)
{
    // 创建监听端点
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
    
    // 打开接受器
    boost::system::error_code ec;
    acceptor_.open(endpoint.protocol(), ec);
    
    if (ec) {
        RENDU_LOG_ERROR("Failed to open acceptor: {}", ec.message());
        throw std::runtime_error("Failed to open acceptor: " + ec.message());
    }
    
    // 设置地址重用
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    
    // 绑定端口
    acceptor_.bind(endpoint, ec);
    
    if (ec) {
        RENDU_LOG_ERROR("Failed to bind to port {}: {}", port, ec.message());
        throw std::runtime_error("Failed to bind to port " + std::to_string(port) + ": " + ec.message());
    }
    
    // 开始监听
    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    
    if (ec) {
        RENDU_LOG_ERROR("Failed to listen: {}", ec.message());
        throw std::runtime_error("Failed to listen: " + ec.message());
    }

    listening_.store(true);

    {
        RENDU_LOG_INFO("Acceptor listening on port {}", port);
    }
}

TcpAcceptor::~TcpAcceptor() {
    close();
}

void TcpAcceptor::async_accept(AcceptCallback callback) {
    if (!listening_.load()) {
        boost::system::error_code ec(boost::system::errc::operation_not_permitted,
                                  boost::system::system_category());
        callback(nullptr, ec);
        return;
    }

    // 创建新的 Socket 用于接受连接
    auto socket = std::make_shared<TcpSocket>(io_);

    // 异步接受（让 acceptor 创建 socket）
    acceptor_.async_accept(
        [callback, socket](const boost::system::error_code& ec, boost::asio::ip::tcp::socket accepted_socket) {
            if (!ec) {
                socket->set_socket(std::move(accepted_socket));
            }
            callback(socket, ec);
        }
    );
}

void TcpAcceptor::close() {
    if (listening_.load()) {
        boost::system::error_code ec;
        acceptor_.close(ec);

        if (ec) {
            RENDU_LOG_WARN("Acceptor close error");
        }

        listening_.store(false);

        RENDU_LOG_INFO("Acceptor closed");
    }
}

bool TcpAcceptor::is_listening() const {
    return listening_.load();
}

boost::asio::ip::tcp::endpoint TcpAcceptor::local_endpoint() const {
    boost::system::error_code ec;
    auto endpoint = acceptor_.local_endpoint(ec);

    if (ec) {
        RENDU_LOG_ERROR("Get local endpoint failed");
    }

    return endpoint;
}

// ============================================================================
// UdpSocket 实现
// ============================================================================

UdpSocket::UdpSocket(io::IoContext& io, uint16_t port)
    : io_(io)
    , socket_(io.native())
{
    // 打开 socket（IPv4 协议）
    boost::system::error_code ec;
    socket_.open(boost::asio::ip::udp::v4(), ec);
    if (ec) {
        RENDU_LOG_ERROR("Failed to open UDP socket: {}", ec.message());
        throw std::runtime_error("Failed to open UDP socket: " + ec.message());
    }

    // 绑定到指定端口（0 表示自动分配）
    bind(port);
}

UdpSocket::~UdpSocket() {
    close();
}

void UdpSocket::async_send_to(const std::vector<byte>& data,
                              const boost::asio::ip::udp::endpoint& endpoint,
                              SendCallback callback) {
    if (!is_open()) {
        boost::system::error_code ec(boost::system::errc::not_connected,
                                  boost::system::system_category());
        callback(ec, 0);
        return;
    }

    // 异步发送
    socket_.async_send_to(
        boost::asio::buffer(data), endpoint,
        [callback](const boost::system::error_code& ec, size_t bytes_sent) {
            callback(ec, bytes_sent);
        }
    );
}

void UdpSocket::async_receive_from(size_t size, ReceiveCallback callback) {
    if (!is_open()) {
        boost::system::error_code ec(boost::system::errc::not_connected,
                                  boost::system::system_category());
        callback(ec, {});
        return;
    }

    // 准备接收缓冲区
    auto buffer = std::make_shared<std::vector<byte>>(size);
    auto endpoint = std::make_shared<boost::asio::ip::udp::endpoint>();

    // 异步接收
    socket_.async_receive_from(
        boost::asio::buffer(*buffer), *endpoint,
        [callback, buffer, endpoint](const boost::system::error_code& ec, size_t bytes_received) {
            if (ec || bytes_received == 0) {
                callback(ec, {});
            } else {
                // 调整缓冲区大小到实际接收的字节数
                buffer->resize(bytes_received);
                callback(ec, std::move(*buffer));
            }
        }
    );
}

void UdpSocket::bind(uint16_t port, const std::string& multicast_addr) {
    boost::system::error_code ec;
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), port);
    socket_.bind(endpoint, ec);

    if (ec) {
        RENDU_LOG_ERROR("Failed to bind UDP socket to port {}: {}", port, ec.message());
        throw std::runtime_error("Failed to bind UDP socket: " + ec.message());
    }

    // 如果指定了多播地址，加入多播组
    if (!multicast_addr.empty()) {
        join_multicast(multicast_addr);
    }

    {
        RENDU_LOG_INFO("UDP socket bound to port {}", port);
    }
}

void UdpSocket::join_multicast(const std::string& multicast_addr) {
    // TODO: 实现多播支持（需要 Boost.Asio 多播选项）
    RENDU_LOG_WARN("Multicast not yet implemented");
}

void UdpSocket::leave_multicast(const std::string& multicast_addr) {
    // TODO: 实现多播支持
    RENDU_LOG_WARN("Multicast not yet implemented");
}

void UdpSocket::set_broadcast(bool enable) {
    boost::system::error_code ec;
    socket_.set_option(boost::asio::socket_base::broadcast(enable), ec);

    if (ec) {
        RENDU_LOG_WARN("Failed to set broadcast option: {}", ec.message());
    }
}

void UdpSocket::close() {
    boost::system::error_code ec;
    socket_.close(ec);

    if (ec) {
        RENDU_LOG_WARN("UDP socket close error");
    }
}

bool UdpSocket::is_open() const {
    return socket_.is_open();
}

boost::asio::ip::udp::endpoint UdpSocket::local_endpoint() const {
    boost::system::error_code ec;
    auto endpoint = socket_.local_endpoint(ec);

    if (ec) {
        RENDU_LOG_ERROR("Get UDP local endpoint failed");
    }

    return endpoint;
}

boost::asio::ip::udp::socket& UdpSocket::native_socket() {
    return socket_;
}

const boost::asio::ip::udp::socket& UdpSocket::native_socket() const {
    return socket_;
}

// ============================================================================
// KCP Socket 占位实现（需要集成 KCP 库）
// ============================================================================

// KCP Socket 将在未来版本中实现
// 需要引入 KCP 库 (https://github.com/skywind3000/kcp)

} // namespace net
END_NAMESPACE_COMMON
