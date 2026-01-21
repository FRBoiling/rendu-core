//
// Created by boil on 2026/1/20.
//

// ============================================================================
// manager.h - 网络管理器
// ============================================================================

#ifndef RENDU_NETWORK_MANAGER_H
#define RENDU_NETWORK_MANAGER_H

#include "common/network/types.h"
#include "common/network/connection.h"
#include "common/asio/io_context.h"
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// Manager - 网络管理器
// ============================================================================

/**
 * @brief 网络管理器（单例模式）
 * 
 * 管理所有网络连接的生命周期
 */
class RC_COMMON_API Manager
{
public:
    /**
     * @brief 获取网络管理器单例
     * @return 网络管理器实例指针
     */
    static Manager* instance() noexcept;

    /**
     * @brief 初始化网络管理器
     * @param ioContext I/O 上下文指针（必须在使用前设置）
     */
    void initialize(Asio::IoContext* ioContext);

    /**
     * @brief 关闭网络管理器
     */
    void shutdown();

    /**
     * @brief 设置网络配置
     * @param config 网络配置
     */
    void setConfig(const NetworkConfig& config);

    /**
     * @brief 获取网络配置
     * @return 网络配置引用
     */
    [[nodiscard]] const NetworkConfig& getConfig() const;

    /**
     * @brief 关闭所有连接
     */
    void closeAllConnections();

    /**
     * @brief 获取活动连接数
     * @return 活动连接数
     */
    [[nodiscard]] size_t getActiveConnectionCount() const;

private:
    /**
     * @brief 私有构造函数
     */
    Manager();

    /**
     * @brief 私有析构函数
     */
    ~Manager();

    // 禁用复制和移动
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(Manager&&) = delete;

    /**
     * @brief 注册连接
     * @param connection 连接指针
     */
    void registerConnection(std::shared_ptr<Connection> connection);

    /**
     * @brief 注销连接
     * @param connection 连接指针
     */
    void unregisterConnection(std::shared_ptr<Connection> connection);

private:
    Asio::IoContext* m_ioContext = nullptr;              ///< I/O 上下文
    std::vector<std::weak_ptr<Connection>> m_connections; ///< 连接列表
    NetworkConfig m_config;                               ///< 网络配置
    std::atomic<bool> m_initialized{false};               ///< 是否已初始化

    static Manager* s_instance;            ///< 单例实例
    static std::mutex s_instanceMutex;     ///< 单例互斥锁

    // 友元类
    friend class Connection;
    friend class Client;
    friend class Server;
};

} // namespace Network

END_NAMESPACE_COMMON

#endif // RENDU_NETWORK_MANAGER_H
