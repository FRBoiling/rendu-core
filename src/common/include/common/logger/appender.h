//
// Created by 沸腾 on 2025/11/7.
//

// ============================================================================
// appender.h - 日志追加器基类
// ============================================================================

#pragma once

#ifndef RENDU_APPENDER_H
#define RENDU_APPENDER_H

#include "common/define.h"
#include <string>
#include <vector>
#include "appender_flags.h"
#include "appender_type.h"
#include "log_level.h"

BEGIN_NAMESPACE_COMMON

// 前置声明
struct LogMessage;

// ============================================================================
// Appender - 日志追加器基类
// 定义日志输出的接口和通用实现
// ============================================================================

class RC_COMMON_API Appender
{
public:
    /**
     * @brief 构造函数
     * @param id 追加器 ID
     * @param name 追加器名称
     * @param level 最低日志级别
     * @param flags 追加器标志
     */
    Appender(
        uint8 id,
        std::string name,
        LogLevel level = LogLevel::LOG_LEVEL_DISABLED,
        AppenderFlags flags = AppenderFlags::APPENDER_FLAGS_NONE
    );

    // 禁用复制和移动
    Appender(Appender const&) = delete;
    Appender(Appender&&) = delete;
    Appender& operator=(Appender const&) = delete;
    Appender& operator=(Appender&&) = delete;

    /**
     * @brief 虚析构函数
     */
    virtual ~Appender();

    /**
     * @brief 获取追加器 ID
     * @return 追加器 ID
     */
    [[nodiscard]] uint8 getId() const;

    /**
     * @brief 获取追加器名称
     * @return 追加器名称引用
     */
    [[nodiscard]] std::string const& getName() const;

    /**
     * @brief 获取追加器类型（纯虚函数）
     * @return 追加器类型
     */
    [[nodiscard]] virtual AppenderType getType() const = 0;

    /**
     * @brief 获取最低日志级别
     * @return 日志级别
     */
    [[nodiscard]] LogLevel getLogLevel() const;

    /**
     * @brief 获取追加器标志
     * @return 追加器标志
     */
    [[nodiscard]] AppenderFlags getFlags() const;

    /**
     * @brief 设置最低日志级别
     * @param level 新的日志级别
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief 写入日志消息
     * @param message 日志消息指针
     */
    void write(LogMessage* message);

    /**
     * @brief 设置领域 ID（可选重写）
     * @param realmId 领域 ID
     */
    virtual void setRealmId(uint32 /*realmId*/) { }

private:
    /**
     * @brief 实际写入日志消息（纯虚函数）
     * @param message 日志消息指针
     */
    virtual void _write(LogMessage const* /*message*/) = 0;

    /**
     * @brief 获取日志级别字符串（静态方法）
     * @param level 日志级别
     * @return 日志级别字符串视图
     */
    [[nodiscard]] static std::string_view getLogLevelString(LogLevel level);

    // 成员变量
    uint8 m_id;              ///< 追加器 ID
    std::string m_name;      ///< 追加器名称
    LogLevel m_level;        ///< 最低日志级别
    AppenderFlags m_flags;   ///< 追加器标志
};

// ============================================================================
// InvalidAppenderArgsException - 无效追加器参数异常
// ============================================================================

class RC_COMMON_API InvalidAppenderArgsException : public std::length_error
{
public:
    /**
     * @brief 构造函数
     * @param message 异常消息
     */
    explicit InvalidAppenderArgsException(std::string const& message)
        : std::length_error(message)
    {
    }
};

// ============================================================================
// 追加器创建函数类型定义
// ============================================================================

typedef Appender* (*AppenderCreatorFn)(
    uint8 id,
    std::string name,
    LogLevel level,
    AppenderFlags flags,
    std::vector<std::string_view> const& extraArgs
);

// ============================================================================
// 追加器创建模板函数
// ============================================================================

/**
 * @brief 创建指定类型的追加器
 * @tparam AppenderImpl 追加器实现类
 * @param id 追加器 ID
 * @param name 追加器名称
 * @param level 最低日志级别
 * @param flags 追加器标志
 * @param extraArgs 额外参数
 * @return 新创建的追加器指针
 */
template <class AppenderImpl>
Appender* CreateAppender(
    uint8 id,
    std::string name,
    LogLevel level,
    AppenderFlags flags,
    std::vector<std::string_view> const& extraArgs)
{
    return new AppenderImpl(id, std::move(name), level, flags, extraArgs);
}

END_NAMESPACE_COMMON

#endif // RENDU_APPENDER_H
