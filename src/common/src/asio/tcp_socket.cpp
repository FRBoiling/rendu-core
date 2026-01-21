//
// Created by boil on 2026/1/21.
//

// ============================================================================
// tcp_socket.cpp - TCP Socket 实现
// ============================================================================

#include "common/asio/tcp_socket.h"
#include "common/asio/io_context.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/ip/host_name.hpp>

BEGIN_NAMESPACE_COMMON

namespace Asio
{
// ============================================================================
// TcpSocket::Impl 实现
// ============================================================================

class TcpSocket::Impl
{
public:
    boost::asio::ip::tcp::socket socket;

    explicit Impl(IoContext& ioContext)
        : socket(*static_cast<boost::asio::io_context*>(ioContext.getNative()))
    {
    }

    ~Impl() = default;
};

// ============================================================================
// TcpSocket 实现
// ============================================================================

TcpSocket::TcpSocket(IoContext& ioContext)
    : m_pImpl(std::make_unique<Impl>(ioContext))
{
}

TcpSocket::~TcpSocket()
{
    close();
}

TcpSocket::TcpSocket(TcpSocket&&) noexcept = default;

TcpSocket& TcpSocket::operator=(TcpSocket&&) noexcept = default;

void TcpSocket::asyncConnect(
    const std::string& host,
    uint16 port,
    std::function<void(const std::error_code&)> onConnect)
{
    if (host.empty())
    {
        if (onConnect)
        {
            boost::asio::post(m_pImpl->socket.get_executor(),
                [onConnect = std::move(onConnect)]() {
                    onConnect(std::make_error_code(std::errc::invalid_argument));
                });
        }
        return;
    }

    try
    {
        using namespace boost::asio::ip;

        // 解析端点
        auto endpoints = tcp::resolver(m_pImpl->socket.get_executor()).resolve(host, std::to_string(port));

        // 异步连接
        boost::asio::async_connect(m_pImpl->socket, endpoints,
            [onConnect = std::move(onConnect)](const boost::system::error_code& ec, const tcp::endpoint&) {
                if (onConnect)
                {
                    onConnect(std::error_code(ec.value(), ec.category()));
                }
            });
    }
    catch (const std::exception&)
    {
        if (onConnect)
        {
            boost::asio::post(m_pImpl->socket.get_executor(),
                [onConnect]() {
                    onConnect(std::make_error_code(std::errc::invalid_argument));
                });
        }
    }
}

void TcpSocket::asyncSend(
    const uint8* data,
    size_t size,
    std::function<void(size_t, const std::error_code&)> onSend)
{
    if (!data || size == 0)
    {
        if (onSend)
        {
            onSend(0, std::make_error_code(std::errc::invalid_argument));
        }
        return;
    }

    boost::asio::async_write(m_pImpl->socket, boost::asio::buffer(data, size),
        [onSend = std::move(onSend)](const boost::system::error_code& ec, size_t bytesTransferred) {
            if (onSend)
            {
                onSend(bytesTransferred, std::error_code(ec.value(), ec.category()));
            }
        });
}

void TcpSocket::asyncSend(
    const std::vector<uint8>& data,
    std::function<void(size_t, const std::error_code&)> onSend)
{
    if (data.empty())
    {
        if (onSend)
        {
            onSend(0, std::make_error_code(std::errc::invalid_argument));
        }
        return;
    }

    boost::asio::async_write(m_pImpl->socket, boost::asio::buffer(data),
        [onSend = std::move(onSend)](const boost::system::error_code& ec, size_t bytesTransferred) {
            if (onSend)
            {
                onSend(bytesTransferred, std::error_code(ec.value(), ec.category()));
            }
        });
}

void TcpSocket::asyncReceive(
    uint8* buffer,
    size_t maxSize,
    std::function<void(size_t, const std::error_code&)> onReceive)
{
    if (!buffer || maxSize == 0)
    {
        if (onReceive)
        {
            onReceive(0, std::make_error_code(std::errc::invalid_argument));
        }
        return;
    }

    m_pImpl->socket.async_read_some(boost::asio::buffer(buffer, maxSize),
        [onReceive = std::move(onReceive)](const boost::system::error_code& ec, size_t bytesTransferred) {
            if (onReceive)
            {
                onReceive(bytesTransferred, std::error_code(ec.value(), ec.category()));
            }
        });
}

void TcpSocket::asyncReceiveExact(
    uint8* buffer,
    size_t size,
    std::function<void(const std::error_code&)> onReceive)
{
    if (!buffer || size == 0)
    {
        if (onReceive)
        {
            onReceive(std::make_error_code(std::errc::invalid_argument));
        }
        return;
    }

    boost::asio::async_read(m_pImpl->socket, boost::asio::buffer(buffer, size),
        [onReceive = std::move(onReceive)](const boost::system::error_code& ec, size_t) {
            if (onReceive)
            {
                onReceive(std::error_code(ec.value(), ec.category()));
            }
        });
}

void TcpSocket::close()
{
    if (m_pImpl && m_pImpl->socket.is_open())
    {
        boost::system::error_code ec;
        m_pImpl->socket.close(ec);
    }
}

void TcpSocket::shutdownWrite()
{
    if (m_pImpl && m_pImpl->socket.is_open())
    {
        boost::system::error_code ec;
        m_pImpl->socket.shutdown(boost::asio::socket_base::shutdown_send, ec);
    }
}

std::string TcpSocket::getRemoteAddress() const
{
    if (!m_pImpl || !m_pImpl->socket.is_open())
    {
        return "";
    }

    boost::system::error_code ec;
    auto endpoint = m_pImpl->socket.remote_endpoint(ec);
    if (ec)
    {
        return "";
    }

    return endpoint.address().to_string();
}

uint16 TcpSocket::getRemotePort() const
{
    if (!m_pImpl || !m_pImpl->socket.is_open())
    {
        return 0;
    }

    boost::system::error_code ec;
    auto endpoint = m_pImpl->socket.remote_endpoint(ec);
    if (ec)
    {
        return 0;
    }

    return endpoint.port();
}

std::string TcpSocket::getLocalAddress() const
{
    if (!m_pImpl || !m_pImpl->socket.is_open())
    {
        return "";
    }

    boost::system::error_code ec;
    auto endpoint = m_pImpl->socket.local_endpoint(ec);
    if (ec)
    {
        return "";
    }

    return endpoint.address().to_string();
}

uint16 TcpSocket::getLocalPort() const
{
    if (!m_pImpl || !m_pImpl->socket.is_open())
    {
        return 0;
    }

    boost::system::error_code ec;
    auto endpoint = m_pImpl->socket.local_endpoint(ec);
    if (ec)
    {
        return 0;
    }

    return endpoint.port();
}

bool TcpSocket::isOpen() const
{
    return m_pImpl && m_pImpl->socket.is_open();
}

void TcpSocket::cancel()
{
    if (m_pImpl && m_pImpl->socket.is_open())
    {
        boost::system::error_code ec;
        m_pImpl->socket.cancel(ec);
    }
}

// ============================================================================
// TcpAcceptor::Impl 实现
// ============================================================================

class TcpAcceptor::Impl
{
public:
    boost::asio::ip::tcp::acceptor acceptor;
    std::string listenAddress;
    uint16 listenPort = 0;

    explicit Impl(IoContext& ioContext)
        : acceptor(*static_cast<boost::asio::io_context*>(ioContext.getNative()))
    {
    }

    ~Impl() = default;
};

// ============================================================================
// TcpAcceptor 实现
// ============================================================================

TcpAcceptor::TcpAcceptor(IoContext& ioContext)
    : m_pImpl(std::make_unique<Impl>(ioContext))
{
}

TcpAcceptor::~TcpAcceptor()
{
    close();
}

TcpAcceptor::TcpAcceptor(TcpAcceptor&&) noexcept = default;

TcpAcceptor& TcpAcceptor::operator=(TcpAcceptor&&) noexcept = default;

bool TcpAcceptor::bind(const std::string& address, uint16 port, bool reuseAddress)
{
    using namespace boost::asio::ip;

    try
    {
        tcp::endpoint endpoint(tcp::v4(), port);
        if (!address.empty() && address != "0.0.0.0.0")
        {
            endpoint = tcp::endpoint(make_address(address), port);
        }

        m_pImpl->acceptor.open(endpoint.protocol());
        m_pImpl->acceptor.set_option(tcp::acceptor::reuse_address(reuseAddress));
        m_pImpl->acceptor.bind(endpoint);
        m_pImpl->acceptor.listen();

        m_pImpl->listenAddress = address.empty() ? "0.0.0.0" : address;
        m_pImpl->listenPort = port;

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void TcpAcceptor::asyncAccept(
    TcpSocket& socket,
    std::function<void(const std::error_code&)> onAccept)
{
    if (!m_pImpl)
    {
        return;
    }

    // 直接访问 TcpSocket::Impl 的 socket 成员（通过友元）
    m_pImpl->acceptor.async_accept(socket.m_pImpl->socket,
        [onAccept = std::move(onAccept)](const boost::system::error_code& ec) {
            if (onAccept)
            {
                onAccept(std::error_code(ec.value(), ec.category()));
            }
        });
}

void TcpAcceptor::close()
{
    if (m_pImpl && m_pImpl->acceptor.is_open())
    {
        boost::system::error_code ec;
        m_pImpl->acceptor.close(ec);
    }
}

bool TcpAcceptor::isOpen() const
{
    return m_pImpl && m_pImpl->acceptor.is_open();
}

std::string TcpAcceptor::getListenAddress() const
{
    return m_pImpl ? m_pImpl->listenAddress : "";
}

uint16 TcpAcceptor::getListenPort() const
{
    return m_pImpl ? m_pImpl->listenPort : 0;
}

} // namespace Asio

END_NAMESPACE_COMMON
