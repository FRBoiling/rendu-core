//
// Created by boil on 2026/1/20.
//

// ============================================================================
// connection.h - 数据库连接类
// ============================================================================

#ifndef RENDU_DATABASE_CONNECTION_H
#define RENDU_DATABASE_CONNECTION_H

#include "common/database/types.h"
#include "common/database/result.h"
#include "common/asio/io_context.h"
#include <string>
#include <memory>
#include <atomic>

BEGIN_NAMESPACE_COMMON

namespace Database
{

// ============================================================================
// Connection - 数据库连接类
// ============================================================================

/**
 * @brief 数据库连接类
 * 
 * 封装数据库连接的生命周期和操作
 */
class RC_COMMON_API Connection
    : public std::enable_shared_from_this<Connection>
{
public:
    /**
     * @brief 构造函数
     * @param ioContext I/O 上下文引用
     * @param config 数据库配置
     */
    explicit Connection(Asio::IoContext& ioContext, const Config& config);

    /**
     * @brief 析构函数
     */
    ~Connection();

    // 禁用复制
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    // 支持移动
    Connection(Connection&&) noexcept = default;
    Connection& operator=(Connection&&) noexcept = default;

    /**
     * @brief 异步执行查询
     * @param query SQL 查询语句
     * @param onQuery 查询完成回调
     */
    void asyncQuery(const std::string& query, OnQueryCallback onQuery);

    /**
     * @brief 异步执行 SQL 语句（INSERT/UPDATE/DELETE）
     * @param sql SQL 语句
     * @param onExecute 执行完成回调
     */
    void asyncExecute(const std::string& sql, OnExecuteCallback onExecute);

    /**
     * @brief 开始事务
     * @return true 如果成功
     */
    bool beginTransaction();

    /**
     * @brief 提交事务
     * @return true 如果成功
     */
    bool commitTransaction();

    /**
     * @brief 回滚事务
     * @return true 如果成功
     */
    bool rollbackTransaction();

    /**
     * @brief 关闭连接
     */
    void close();

    /**
     * @brief 获取连接状态
     * @return 连接状态
     */
    [[nodiscard]] ConnectionStatus getStatus() const;

    /**
     * @brief 获取连接配置
     * @return 连接配置引用
     */
    [[nodiscard]] const Config& getConfig() const;

    /**
     * @brief 设置连接回调
     * @param onClose 关闭回调
     */
    void setOnCloseCallback(OnCloseCallback onClose);

    /**
     * @brief 获取连接 ID
     * @return 连接 ID
     */
    [[nodiscard]] uint64 getConnectionId() const { return m_connectionId; }

    /**
     * @brief 是否处于事务中
     * @return true 如果处于事务中
     */
    [[nodiscard]] bool isInTransaction() const { return m_inTransaction; }

private:
    /**
     * @brief 生成连接 ID
     * @return 连接 ID
     */
    static uint64 generateConnectionId();

    /**
     * @brief 设置连接状态
     * @param status 新状态
     */
    void setStatus(ConnectionStatus status);

    /**
     * @brief 触发关闭回调
     * @param ec 错误码
     */
    void notifyClose(const std::error_code& ec);

private:
    Asio::IoContext& m_ioContext;                 ///< I/O 上下文引用
    Config m_config;                              ///< 数据库配置
    uint64 m_connectionId;                        ///< 连接唯一 ID
    std::atomic<ConnectionStatus> m_status{ConnectionStatus::Disconnected}; ///< 连接状态

    OnCloseCallback m_onClose;                    ///< 关闭回调

    bool m_inTransaction = false;                 ///< 是否在事务中
    std::atomic<bool> m_inPool{false};            ///< 是否在连接池中

    // PIMPL 实现
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace Database

END_NAMESPACE_COMMON

#endif // RENDU_DATABASE_CONNECTION_H
