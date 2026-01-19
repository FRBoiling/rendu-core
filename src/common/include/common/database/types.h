//
// Created by boil on 2026/1/20.
//

// ============================================================================
// types.h - 数据库模块类型定义
// ============================================================================

#ifndef RENDU_DATABASE_TYPES_H
#define RENDU_DATABASE_TYPES_H

#include "common/define.h"
#include <cstdint>
#include <string>
#include <functional>
#include <system_error>
#include <map>
#include <variant>

BEGIN_NAMESPACE_COMMON

namespace Database
{

// ============================================================================
// 前置声明
// ============================================================================

class Connection;
class QueryResult;

// ============================================================================
// 数据库类型枚举
// ============================================================================

/**
 * @brief 数据库类型
 */
enum class DbType : uint8
{
    MySQL      = 0,  ///< MySQL
    PostgreSQL = 1,  ///< PostgreSQL
    SQLite     = 2,  ///< SQLite
    Oracle     = 3,  ///< Oracle
    SQLServer  = 4   ///< SQL Server
};

// ============================================================================
// 数据库连接状态枚举
// ============================================================================

/**
 * @brief 数据库连接状态
 */
enum class ConnectionStatus : uint8
{
    Disconnected = 0,  ///< 已断开连接
    Connecting    = 1,  ///< 正在连接
    Connected     = 2,  ///< 已连接
    Error         = 3   ///< 连接错误
};

// ============================================================================
// 查询结果值类型
// ============================================================================

/**
 * @brief 查询结果值类型
 */
using Value = std::variant<
    int64,           ///< 整数
    double,          ///< 浮点数
    std::string,     ///< 字符串
    std::nullptr_t   ///< NULL 值
>;

/**
 * @brief 查询行（字段名到值的映射）
 */
using Row = std::map<std::string, Value>;

/**
 * @brief 查询结果集（多行）
 */
using ResultSet = std::vector<Row>;

// ============================================================================
// 数据库事件回调类型定义
// ============================================================================

/**
 * @brief 连接建立回调
 * @param connection 连接指针
 * @param ec 错误码（成功时为空）
 */
using OnConnectCallback = std::function<void(std::shared_ptr<Connection>, const std::error_code&)>;

/**
 * @brief 查询完成回调
 * @param connection 连接指针
 * @param result 查询结果指针（查询失败时为空）
 * @param ec 错误码
 */
using OnQueryCallback = std::function<void(std::shared_ptr<Connection>, std::shared_ptr<QueryResult>, const std::error_code&)>;

/**
 * @brief 执行完成回调
 * @param connection 连接指针
 * @param affectedRows 受影响的行数
 * @param ec 错误码
 */
using OnExecuteCallback = std::function<void(std::shared_ptr<Connection>, uint64 affectedRows, const std::error_code&)>;

/**
 * @brief 连接关闭回调
 * @param connection 连接指针
 * @param ec 错误码
 */
using OnCloseCallback = std::function<void(std::shared_ptr<Connection>, const std::error_code&)>;

// ============================================================================
// 数据库配置结构体
// ============================================================================

/**
 * @brief 数据库连接配置
 */
struct Config
{
    DbType type = DbType::MySQL;    ///< 数据库类型
    std::string host = "localhost"; ///< 主机地址
    uint16 port = 3306;             ///< 端口号
    std::string database;           ///< 数据库名称
    std::string username;           ///< 用户名
    std::string password;           ///< 密码
    uint32 timeout = 30;            ///< 连接超时（秒）
    bool autoReconnect = true;      ///< 自动重连
    uint32 poolSize = 10;           ///< 连接池大小
};

} // namespace Database

END_NAMESPACE_COMMON

#endif // RENDU_DATABASE_TYPES_H
