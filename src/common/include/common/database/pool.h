//
// Created by boil on 2026/1/20.
//

// ============================================================================
// pool.h - 数据库连接池
// ============================================================================

#ifndef RENDU_DATABASE_POOL_H
#define RENDU_DATABASE_POOL_H

#include "common/database/types.h"
#include "common/database/connection.h"
#include "common/asio/io_context.h"
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Database
{

// ============================================================================
// ConnectionPool - 数据库连接池
// ============================================================================

/**
 * @brief 数据库连接池类
 * 
 * 管理多个数据库连接，提供连接复用
 */
class RC_COMMON_API Pool
{
public:
    /**
     * @brief 构造函数
     * @param name 连接池名称
     * @param ioContext I/O 上下文引用
     * @param config 数据库配置
     * @param poolSize 连接池大小
     */
    Pool(
        const std::string& name,
        Asio::IoContext& ioContext,
        const Config& config,
        uint32 poolSize);

    /**
     * @brief 析构函数
     */
    ~Pool();

    // 禁用复制和移动
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool(Pool&&) = delete;
    Pool& operator=(Pool&&) = delete;

    /**
     * @brief 初始化连接池
     * @param onReady 连接池就绪回调
     */
    void initialize(OnConnectCallback onReady);

    /**
     * @brief 从连接池获取一个连接
     * @param timeout 超时时间（毫秒，0 表示不等待）
     * @return 连接指针（池为空或超时时返回 nullptr）
     */
    std::shared_ptr<Connection> acquire(uint32 timeout = 0);

    /**
     * @brief 归还连接到连接池
     * @param connection 连接指针
     */
    void release(std::shared_ptr<Connection> connection);

    /**
     * @brief 关闭连接池
     */
    void close();

    /**
     * @brief 获取连接池名称
     * @return 连接池名称
     */
    [[nodiscard]] std::string getName() const;

    /**
     * @brief 获取连接池大小
     * @return 连接池大小
     */
    [[nodiscard]] uint32 getPoolSize() const;

    /**
     * @brief 获取空闲连接数
     * @return 空闲连接数
     */
    [[nodiscard]] uint32 getAvailableCount() const;

    /**
     * @brief 获取活动连接数
     * @return 活动连接数
     */
    [[nodiscard]] uint32 getActiveCount() const;

    /**
     * @brief 是否已初始化
     * @return true 如果已初始化
     */
    [[nodiscard]] bool isInitialized() const { return m_initialized; }

private:
    /**
     * @brief 创建新连接
     * @param callback 连接回调
     */
    void createConnection(OnConnectCallback callback);

    /**
     * @brief 检查所有连接是否已就绪
     */
    void checkReady();

private:
    std::string m_name;                       ///< 连接池名称
    Asio::IoContext& m_ioContext;            ///< I/O 上下文引用
    Config m_config;                          ///< 数据库配置
    uint32 m_poolSize;                        ///< 连接池大小

    std::vector<std::shared_ptr<Connection>> m_connections; ///< 所有连接
    std::queue<std::shared_ptr<Connection>> m_available;      ///< 可用连接队列
    mutable std::mutex m_mutex;               ///< 互斥锁
    std::condition_variable m_cv;            ///< 条件变量

    std::atomic<uint32> m_activeCount{0};    ///< 活动连接数
    std::atomic<bool> m_initialized{false};   ///< 是否已初始化
    std::atomic<bool> m_closing{false};      ///< 是否正在关闭

    OnConnectCallback m_onReady;              ///< 就绪回调
};

} // namespace Database

END_NAMESPACE_COMMON

#endif // RENDU_DATABASE_POOL_H
