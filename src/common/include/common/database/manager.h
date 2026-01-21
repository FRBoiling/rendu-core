//
// Created by boil on 2026/1/20.
//

// ============================================================================
// manager.h - 数据库管理器
// ============================================================================

#ifndef RENDU_DATABASE_MANAGER_H
#define RENDU_DATABASE_MANAGER_H

#include "common/database/types.h"
#include "common/database/pool.h"
#include "common/asio/io_context.h"
#include <string>
#include <map>
#include <memory>
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Database
{

// ============================================================================
// Manager - 数据库管理器
// ============================================================================

/**
 * @brief 数据库管理器（单例模式）
 * 
 * 管理所有数据库连接和连接池
 */
class RC_COMMON_API Manager
{
public:
    /**
     * @brief 获取数据库管理器单例
     * @return 数据库管理器实例指针
     */
    static Manager* instance() noexcept;

    /**
     * @brief 初始化数据库管理器
     * @param ioContext I/O 上下文指针
     */
    void initialize(Asio::IoContext* ioContext);

    /**
     * @brief 关闭数据库管理器
     */
    void shutdown();

    /**
     * @brief 创建连接池
     * @param name 连接池名称
     * @param config 数据库配置
     * @param poolSize 连接池大小
     * @param onReady 就绪回调
     * @return 连接池指针
     */
    std::shared_ptr<Pool> createPool(
        const std::string& name,
        const Config& config,
        uint32 poolSize,
        OnConnectCallback onReady);

    /**
     * @brief 获取连接池
     * @param name 连接池名称
     * @return 连接池指针（不存在时返回 nullptr）
     */
    [[nodiscard]] std::shared_ptr<Pool> getPool(const std::string& name);

    /**
     * @brief 从连接池获取连接
     * @param poolName 连接池名称
     * @param timeout 超时时间（毫秒）
     * @return 连接指针
     */
    std::shared_ptr<Connection> acquire(const std::string& poolName, uint32 timeout = 0);

    /**
     * @brief 归还连接到连接池
     * @param poolName 连接池名称
     * @param connection 连接指针
     */
    void release(const std::string& poolName, std::shared_ptr<Connection> connection);

    /**
     * @brief 关闭所有连接池
     */
    void closeAll();

    /**
     * @brief 获取连接池数量
     * @return 连接池数量
     */
    [[nodiscard]] size_t getPoolCount() const;

    /**
     * @brief 获取所有连接池名称
     * @return 连接池名称列表
     */
    [[nodiscard]] std::vector<std::string> getPoolNames() const;

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

private:
    Asio::IoContext* m_ioContext = nullptr;    ///< I/O 上下文
    std::map<std::string, std::shared_ptr<Pool>> m_pools; ///< 连接池映射
    std::atomic<bool> m_initialized{false};     ///< 是否已初始化
};

} // namespace Database

END_NAMESPACE_COMMON

#endif // RENDU_DATABASE_MANAGER_H
