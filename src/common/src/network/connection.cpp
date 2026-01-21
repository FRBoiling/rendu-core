//
// Created by boil on 2026/1/21.
//

// ============================================================================
// connection.cpp - 网络连接实现
// ============================================================================

#include "common/network/connection.h"
#include "common/network/manager.h"
#include "common/asio/post.h"
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Connection 实现
// ============================================================================

// 静态成员定义
std::atomic<uint64> Connection::s_nextConnectionId{1};

Connection::Connection(Asio::IoContext& ioContext)
    : m_ioContext(ioContext)
    , m_socket(ioContext)
    , m_isServer(false)
    , m_connectionId(generateConnectionId())
    , m_recvBuffer(RECV_BUFFER_SIZE)
{
}

Connection::Connection(Asio::IoContext& ioContext, Asio::TcpSocket socket)
    : m_ioContext(ioContext)
    , m_socket(std::move(socket))
    , m_isServer(true)
    , m_connectionId(generateConnectionId())
    , m_state(ConnectionState::Connected)
    , m_recvBuffer(RECV_BUFFER_SIZE)
{
}

Connection::~Connection()
{
    // 析构时直接清理，不调用 close() 以避免使用 shared_from_this()
    auto oldState = m_state.exchange(ConnectionState::Disconnected, std::memory_order_acq_rel);
    if (oldState != ConnectionState::Disconnected)
    {
        // 取消所有异步操作
        m_socket.cancel();

        // 关闭 socket
        m_socket.close();

        // 不触发关闭回调，因为对象正在销毁
    }
}

uint64 Connection::generateConnectionId()
{
    return s_nextConnectionId.fetch_add(1, std::memory_order_relaxed);
}

void Connection::asyncSend(const uint8* data, size_t size, OnSendCallback onSend)
{
    if (!data || size == 0)
    {
        Asio::Post(m_ioContext, [this, onSend]() {
            if (onSend)
            {
                onSend(shared_from_this(), 0, std::make_error_code(std::errc::invalid_argument));
            }
        });
        return;
    }

    if (m_state != ConnectionState::Connected)
    {
        Asio::Post(m_ioContext, [this, onSend]() {
            if (onSend)
            {
                onSend(shared_from_this(), 0, std::make_error_code(std::errc::not_connected));
            }
        });
        return;
    }

    m_pendingSend = onSend;
    m_socket.asyncSend(data, size,
        [this](size_t bytesTransferred, const std::error_code& ec) {
            Asio::Post(m_ioContext, [this, bytesTransferred, ec]() {
                handleSend(bytesTransferred, ec);
            });
        });
}

void Connection::asyncSend(const std::string& str, OnSendCallback onSend)
{
    asyncSend(reinterpret_cast<const uint8*>(str.data()), str.size(), onSend);
}

void Connection::asyncReceive(OnReceiveCallback onReceive)
{
    if (m_state != ConnectionState::Connected)
    {
        if (onReceive)
        {
            Asio::Post(m_ioContext, [this, onReceive]() {
                onReceive(shared_from_this(), nullptr, 0, std::make_error_code(std::errc::not_connected));
            });
        }
        return;
    }

    m_onReceive = onReceive;
    m_socket.asyncReceive(m_recvBuffer.data(), m_recvBuffer.size(),
        [this](size_t bytesTransferred, const std::error_code& ec) {
            Asio::Post(m_ioContext, [this, bytesTransferred, ec]() {
                handleReceive(bytesTransferred, ec);
            });
        });
}

void Connection::close()
{
    auto oldState = m_state.exchange(ConnectionState::Disconnected, std::memory_order_acq_rel);
    if (oldState == ConnectionState::Disconnected)
    {
        return;
    }

    // 取消所有异步操作
    m_socket.cancel();

    // 关闭 socket
    m_socket.close();

    // 触发关闭回调 - 使用 shared_ptr 确保对象在回调执行期间仍然存活
    Asio::Post(m_ioContext, [self = shared_from_this()]() {
        self->notifyClose(std::error_code());
    });
}

ConnectionState Connection::getState() const
{
    return m_state.load(std::memory_order_acquire);
}

std::string Connection::getRemoteAddress() const
{
    return m_socket.getRemoteAddress();
}

uint16 Connection::getRemotePort() const
{
    return m_socket.getRemotePort();
}

void Connection::setOnCloseCallback(OnCloseCallback onClose)
{
    m_onClose = onClose;
}

void Connection::setState(ConnectionState state)
{
    m_state.store(state, std::memory_order_release);
}

void Connection::notifyClose(const std::error_code& ec)
{
    auto onClose = m_onClose;
    if (onClose)
    {
        onClose(shared_from_this(), ec);
    }
}

void Connection::handleSend(size_t bytesTransferred, const std::error_code& ec)
{
    auto onSend = m_pendingSend;
    m_pendingSend = nullptr;

    if (onSend)
    {
        onSend(shared_from_this(), bytesTransferred, ec);
    }

    // 发送错误时关闭连接
    if (ec && ec != std::errc::operation_canceled)
    {
        close();
    }
}

void Connection::handleReceive(size_t bytesTransferred, const std::error_code& ec)
{
    auto onReceive = m_onReceive;

    if (ec)
    {
        // 错误时关闭连接
        if (onReceive)
        {
            onReceive(shared_from_this(), m_recvBuffer.data(), bytesTransferred, ec);
        }
        if (ec != std::errc::operation_canceled)
        {
            close();
        }
        return;
    }

    // 继续接收
    asyncReceive(onReceive);

    // 调用接收回调
    if (onReceive)
    {
        onReceive(shared_from_this(), m_recvBuffer.data(), bytesTransferred, ec);
    }
}

} // namespace Network

END_NAMESPACE_COMMON
