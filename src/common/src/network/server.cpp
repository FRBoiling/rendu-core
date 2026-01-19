//
// Created by boil on 2026/1/21.
//

// ============================================================================
// server.cpp - 网络服务器实现
// ============================================================================

#include "common/network/server.h"
#include "common/network/manager.h"
#include <functional>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Server 实现
// ============================================================================

Server::Server(Asio::IoContext& ioContext)
    : m_ioContext(ioContext)
    , m_acceptSocket(std::make_unique<Asio::TcpSocket>(ioContext))
{
}

Server::~Server()
{
    stop();
}

bool Server::start(
    const std::string& host,
    uint16 port,
    OnAcceptCallback onAccept)
{
    if (m_running.load(std::memory_order_acquire))
    {
        return false;
    }

    m_acceptor = std::make_unique<Asio::TcpAcceptor>(m_ioContext);

    if (!m_acceptor->bind(host, port, true))
    {
        m_acceptor.reset();
        return false;
    }

    m_listenAddress = host.empty() ? "0.0.0.0" : host;
    m_listenPort = port;
    m_onAccept = onAccept;
    m_running.store(true, std::memory_order_release);

    // 开始接受连接
    startAccept();

    return true;
}

void Server::stop()
{
    if (!m_running.load(std::memory_order_acquire))
    {
        return;
    }

    m_running.store(false, std::memory_order_release);

    // 关闭接受器
    if (m_acceptor)
    {
        m_acceptor->close();
    }

    // 关闭所有连接
    std::vector<std::shared_ptr<Connection>> connections;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        connections = std::move(m_connections);
        m_connections.clear();
    }

    for (auto& connection : connections)
    {
        if (connection)
        {
            connection->close();
        }
    }
}

bool Server::isRunning() const
{
    return m_running.load(std::memory_order_acquire);
}

std::string Server::getListenAddress() const
{
    return m_listenAddress;
}

uint16 Server::getListenPort() const
{
    return m_listenPort;
}

std::vector<std::shared_ptr<Connection>> Server::getConnections() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_connections;
}

size_t Server::getConnectionCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_connections.size();
}

void Server::startAccept()
{
    m_acceptor->asyncAccept(*m_acceptSocket,
        [this](const std::error_code& ec) {
            handleAccept(ec);
        });
}

void Server::handleAccept(const std::error_code& ec)
{
    // 如果停止了，不再接受
    if (!m_running.load(std::memory_order_acquire))
    {
        return;
    }

    if (ec)
    {
        // 接受错误
        if (m_onAccept)
        {
            m_onAccept(nullptr, ec);
        }
        return;
    }

    // 创建新连接
    auto connection = std::make_shared<Connection>(m_ioContext, std::move(*m_acceptSocket));

    // 创建新的接受 socket
    m_acceptSocket = std::make_unique<Asio::TcpSocket>(m_ioContext);

    // 添加到连接列表
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.push_back(connection);
    }

    // 注册到管理器
    auto* manager = Manager::instance();
    if (manager)
    {
        manager->registerConnection(connection);
    }

    // 设置关闭回调以自动移除连接
    connection->setOnCloseCallback([this](std::shared_ptr<Connection> conn, const std::error_code&) {
        removeConnection(conn);
    });

    // 调用接受回调
    if (m_onAccept)
    {
        m_onAccept(connection, ec);
    }

    // 继续接受新连接
    if (m_running.load(std::memory_order_acquire))
    {
        startAccept();
    }
}

void Server::removeConnection(std::shared_ptr<Connection> connection)
{
    if (!connection)
    {
        return;
    }

    // 从管理器注销
    auto* manager = Manager::instance();
    if (manager)
    {
        manager->unregisterConnection(connection);
    }

    // 从列表移除
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove(m_connections.begin(), m_connections.end(), connection);
    m_connections.erase(it, m_connections.end());
}

} // namespace Network

END_NAMESPACE_COMMON
