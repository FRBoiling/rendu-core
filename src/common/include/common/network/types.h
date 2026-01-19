//
// Created by boil on 2026/1/20.
//

// ============================================================================
// types.h - 网络模块类型定义
// ============================================================================

#ifndef RENDU_NETWORK_TYPES_H
#define RENDU_NETWORK_TYPES_H

#include "common/define.h"
#include <cstdint>
#include <string>
#include <functional>
#include <system_error>

BEGIN_NAMESPACE_COMMON

namespace Network
{

// ============================================================================
// 前置声明
// ============================================================================

class Connection;

// ============================================================================
// 网络连接状态枚举
// ============================================================================

/**
 * @brief 网络连接状态
 */
enum class ConnectionState : uint8
{
    Disconnected = 0,  ///< 已断开连接
    Connecting    = 1,  ///< 正在连接
    Connected     = 2,  ///< 已连接
    Disconnecting = 3   ///< 正在断开连接
};

// ============================================================================
// 网络事件回调类型定义
// ============================================================================

/**
 * @brief 连接建立回调
 * @param connection 连接指针
 * @param ec 错误码（成功时为空）
 */
using OnConnectCallback = std::function<void(std::shared_ptr<Connection>, const std::error_code&)>;

/**
 * @brief 数据接收回调
 * @param connection 连接指针
 * @param data 接收到的数据
 * @param size 数据大小
 * @param ec 错误码
 */
using OnReceiveCallback = std::function<void(std::shared_ptr<Connection>, const uint8* data, size_t size, const std::error_code&)>;

/**
 * @brief 数据发送完成回调
 * @param connection 连接指针
 * @param bytesTransferred 发送的字节数
 * @param ec 错误码
 */
using OnSendCallback = std::function<void(std::shared_ptr<Connection>, size_t bytesTransferred, const std::error_code&)>;

/**
 * @brief 连接关闭回调
 * @param connection 连接指针
 * @param ec 错误码
 */
using OnCloseCallback = std::function<void(std::shared_ptr<Connection>, const std::error_code&)>;

/**
 * @brief 接受连接回调（服务器端）
 * @param connection 新连接指针
 * @param ec 错误码
 */
using OnAcceptCallback = std::function<void(std::shared_ptr<Connection>, const std::error_code&)>;

// ============================================================================
// 网络配置结构体
// ============================================================================

/**
 * @brief 网络配置
 */
struct NetworkConfig
{
    uint32 connectionTimeout = 30;     ///< 连接超时（秒）
    uint32 sendTimeout = 10;            ///< 发送超时（秒）
    uint32 receiveTimeout = 10;         ///< 接收超时（秒）
    uint32 maxConnections = 1000;       ///< 最大连接数
    bool   autoReconnect = false;       ///< 自动重连
    size_t sendBufferSize = 65536;      ///< 发送缓冲区大小
    size_t receiveBufferSize = 65536;   ///< 接收缓冲区大小
};

} // namespace Network

END_NAMESPACE_COMMON

#endif // RENDU_NETWORK_TYPES_H
