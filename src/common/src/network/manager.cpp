//
// Created by boil on 2026/1/21.
//

// ============================================================================
// manager.cpp - 网络管理器实现
// ============================================================================

#include "common/network/manager.h"
#include <mutex>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Manager 实现
// ============================================================================

// 静态成员定义
Manager* Manager::s_instance = nullptr;
std::mutex Manager::s_instanceMutex;

Manager::Manager() = default;

Manager::~Manager()
{
    shutdown();
}

Manager* Manager::instance() noexcept
{
    std::lock_guard<std::mutex> lock(s_instanceMutex);
    if (s_instance == nullptr)
    {
        s_instance = new Manager();
    }
    return s_instance;
}

void Manager::initialize(Asio::IoContext* ioContext)
{
    if (ioContext == nullptr)
    {
        return;
    }

    m_ioContext = ioContext;
    m_initialized = true;
}

void Manager::shutdown()
{
    if (!m_initialized)
    {
        return;
    }

    closeAllConnections();

    m_ioContext = nullptr;
    m_initialized = false;
}

void Manager::setConfig(const NetworkConfig& config)
{
    m_config = config;
}

const NetworkConfig& Manager::getConfig() const
{
    return m_config;
}

void Manager::closeAllConnections()
{
    std::vector<std::weak_ptr<Connection>> connections;
    {
        std::lock_guard<std::mutex> lock(s_instanceMutex);
        connections = m_connections;
        m_connections.clear();
    }

    for (auto& weakConn : connections)
    {
        if (auto conn = weakConn.lock())
        {
            conn->close();
        }
    }
}

size_t Manager::getActiveConnectionCount() const
{
    size_t count = 0;
    std::lock_guard<std::mutex> lock(s_instanceMutex);

    for (const auto& weakConn : m_connections)
    {
        if (auto conn = weakConn.lock())
        {
            count++;
        }
    }

    return count;
}

void Manager::registerConnection(std::shared_ptr<Connection> connection)
{
    if (!connection)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(s_instanceMutex);
    m_connections.push_back(connection);
}

void Manager::unregisterConnection(std::shared_ptr<Connection> connection)
{
    if (!connection)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(s_instanceMutex);
    auto it = std::remove_if(m_connections.begin(), m_connections.end(),
        [&connection](const std::weak_ptr<Connection>& weakConn) {
            return weakConn.lock() == connection;
        });
    m_connections.erase(it, m_connections.end());
}

} // namespace Network

END_NAMESPACE_COMMON
