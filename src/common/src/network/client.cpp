//
// Created by boil on 2026/1/21.
//

// ============================================================================
// client.cpp - 网络客户端实现
// ============================================================================

#include "common/network/client.h"
#include "common/network/manager.h"
#include <functional>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Client 实现
// ============================================================================

Client::Client(Asio::IoContext& ioContext)
    : m_ioContext(ioContext)
    , m_socket(std::make_unique<Asio::TcpSocket>(ioContext))
{
}

Client::~Client()
{
    disconnect();
}

std::shared_ptr<Connection> Client::asyncConnect(
    const std::string& host,
    uint16 port,
    OnConnectCallback onConnect)
{
    auto connection = std::make_shared<Connection>(m_ioContext, std::move(*m_socket));
    m_socket = std::make_unique<Asio::TcpSocket>(m_ioContext);

    // 异步连接
    connection->getSocket().asyncConnect(host, port,
        [connection, onConnect](const std::error_code& ec) mutable {
            if (!ec)
            {
                // 连接成功
                connection->markConnected();

                // 注册到管理器
                auto* manager = Manager::instance();
                if (manager)
                {
                    manager->registerConnection(connection);
                }
            }

            if (onConnect)
            {
                onConnect(connection, ec);
            }

            // 连接失败时清理
            if (ec)
            {
                connection->close();
            }
        });

    return connection;
}

std::shared_ptr<Connection> Client::getConnection() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_connection;
}

void Client::disconnect()
{
    std::shared_ptr<Connection> connection;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        connection = std::move(m_connection);
    }

    if (connection)
    {
        // 从管理器注销
        auto* manager = Manager::instance();
        if (manager)
        {
            manager->unregisterConnection(connection);
        }

        connection->close();
    }
}

bool Client::isConnected() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_connection)
    {
        return false;
    }

    return m_connection->getState() == ConnectionState::Connected;
}

} // namespace Network

END_NAMESPACE_COMMON
