//
// Created by 沸腾 on 2025/11/3.
//

// ============================================================================
// log.h - 日志系统主入口
// ============================================================================

#pragma once

#ifndef RENDU_LOG_H
#define RENDU_LOG_H

#include <unordered_map>
#include <memory>
#include "appender.h"
#include "appender_type.h"
#include "logger.h"
#include "log_level.h"
#include "common/asio/io_context.h"
#include "common/utils/string_format.h"

BEGIN_NAMESPACE_COMMON

// 根日志器名称
#define LOGGER_ROOT "root"

// ============================================================================
// Log - 日志系统主类（单例模式）
// ============================================================================

class RC_COMMON_API Log
{
private:
    /**
     * @brief 私有构造函数
     */
    Log();

    /**
     * @brief 私有析构函数
     */
    ~Log();

public:
    // 禁用复制和移动
    Log(Log const&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log const&) = delete;
    Log& operator=(Log&&) = delete;

    /**
     * @brief 获取日志系统单例
     * @return 日志系统实例指针
     */
    static Log* instance() noexcept;

    /**
     * @brief 初始化日志系统
     * @param ioContext I/O 上下文指针（用于异步日志）
     */
    void initialize(Asio::IoContext* ioContext);

    /**
     * @brief 关闭日志系统
     */
    void close();

    /**
     * @brief 设置日志级别
     * @param name 日志器或追加器名称
     * @param level 日志级别
     * @param isLogger true 表示设置日志器，false 表示设置追加器
     * @return true 如果设置成功，否则 false
     */
    bool setLogLevel(std::string const& name, int32 level, bool isLogger = true);

    /**
     * @brief 注册追加器类型
     * @tparam AppenderImpl 追加器实现类
     */
    template <class AppenderImpl>
    void registerAppender()
    {
        this->registerAppender(static_cast<uint8>(AppenderImpl::type), &CreateAppender<AppenderImpl>);
    }

    /**
     * @brief 输出日志消息到指定日志器
     * @tparam Args 格式化参数类型
     * @param logger 日志器指针
     * @param filter 过滤器类型
     * @param level 日志级别
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void outMessageTo(
        Logger const* logger,
        std::string_view filter,
        LogLevel level,
        Utils::FormatString<Args...> fmt,
        Args&&... args) noexcept
    {
        this->outMessageImpl(logger, filter, level, fmt, Utils::MakeFormatArgs(args...));
    }

    /**
     * @brief 检查是否应该记录日志
     * @param type 日志器类型
     * @param level 日志级别
     * @return true 如果应该记录，否则 false
     */
    [[nodiscard]] bool shouldLog(std::string_view type, LogLevel level) const noexcept;

    /**
     * @brief 获取启用的日志器
     * @param type 日志器类型
     * @param level 日志级别
     * @return 日志器指针，如果未启用则返回 nullptr
     */
    [[nodiscard]] Logger const* getEnabledLogger(std::string_view type, LogLevel level) const noexcept;

    /**
     * @brief 根据名称获取追加器
     * @param name 追加器名称
     * @return 追加器指针，未找到则返回 nullptr
     */
    [[nodiscard]] Appender* getAppenderByName(std::string_view name);

    /**
     * @brief 获取日志目录
     * @return 日志目录路径
     */
    [[nodiscard]] std::string const& getLogsDir() const { return m_logsDir; }

    /**
     * @brief 获取日志时间戳
     * @return 时间戳字符串
     */
    [[nodiscard]] std::string const& getLogsTimestamp() const { return m_logsTimestamp; }

    /**
     * @brief 设置为同步模式（非线程安全）
     * 注意：只能在主线程中所有线程都退出后调用
     */
    void setSynchronous();

    /**
     * @brief 从配置行创建追加器
     * @param name 追加器名称
     * @param options 配置选项字符串
     */
    void createAppenderFromConfigLine(std::string const& name, std::string const& options);

    /**
     * @brief 从配置行创建日志器
     * @param name 日志器名称
     * @param options 配置选项字符串
     */
    void createLoggerFromConfigLine(std::string const& name, std::string const& options);

private:
    /**
     * @brief 获取时间戳字符串
     * @return 时间戳字符串
     */
    static std::string getTimestampStr();

    /**
     * @brief 根据类型获取日志器
     * @param type 日志器类型
     * @return 日志器指针，未找到则返回 nullptr
     */
    [[nodiscard]] Logger const* getLoggerByType(std::string_view type) const;

    /**
     * @brief 生成下一个追加器 ID
     * @return 追加器 ID
     */
    [[nodiscard]] uint8 nextAppenderId();

    /**
     * @brief 注册追加器创建函数
     * @param index 追加器类型索引
     * @param appenderCreateFn 追加器创建函数
     */
    void registerAppender(uint8 index, AppenderCreatorFn appenderCreateFn);

    /**
     * @brief 实现输出日志消息
     * @param logger 日志器指针
     * @param filter 过滤器类型
     * @param level 日志级别
     * @param messageFormat 消息格式
     * @param messageFormatArgs 格式化参数
     */
    void outMessageImpl(
        Logger const* logger,
        std::string_view filter,
        LogLevel level,
        Utils::FormatStringView messageFormat,
        Utils::FormatArgs messageFormatArgs) const noexcept;

    // 成员变量
    std::unordered_map<uint8, AppenderCreatorFn> m_appenderFactory;    ///< 追加器工厂
    std::unordered_map<uint8, std::unique_ptr<Appender>> m_appenders;  ///< 追加器映射
    std::unordered_map<std::string_view, std::unique_ptr<Logger>> m_loggers; ///< 日志器映射

    uint8 m_appenderId;        ///< 下一个追加器 ID
    LogLevel m_lowestLogLevel; ///< 最低日志级别

    std::string m_logsDir;        ///< 日志目录
    std::string m_logsTimestamp;  ///< 日志时间戳

    Asio::IoContext* m_ioContext = nullptr;            ///< I/O 上下文
    std::unique_ptr<Asio::IoContext::Strand> m_strand;  ///< 串行执行器（用于线程安全）
};

// ============================================================================
// 日志消息核心函数
// ============================================================================

/**
 * @brief 输出日志消息的核心实现
 * @tparam Args 格式化参数类型
 * @param filterType 过滤器类型
 * @param level 日志级别
 * @param message 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void LogMessageCore(
    std::string_view filterType,
    LogLevel level,
    Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    Log* logInstance = Log::instance();
    if (Logger const* loggerInstance = logInstance->getEnabledLogger(filterType, level))
    {
        logInstance->outMessageTo(loggerInstance, filterType, level,
            message, std::forward<Args>(args)...);
    }
}

// ============================================================================
// 日志级别宏函数
// ============================================================================

/**
 * @brief 输出 TRACE 级别日志
 */
template <typename... Args>
void LogTrace(
    std::string_view filterType,
    Rendu::Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    LogMessageCore(filterType, LogLevel::LOG_LEVEL_TRACE, message, std::forward<Args>(args)...);
}

/**
 * @brief 输出 DEBUG 级别日志
 */
template <typename... Args>
void LogDebug(
    std::string_view filterType,
    Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    LogMessageCore(filterType, LogLevel::LOG_LEVEL_DEBUG, message, std::forward<Args>(args)...);
}

/**
 * @brief 输出 INFO 级别日志
 */
template <typename... Args>
void LogInfo(
    std::string_view filterType,
    Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    LogMessageCore(filterType, LogLevel::LOG_LEVEL_INFO, message, std::forward<Args>(args)...);
}

/**
 * @brief 输出 WARN 级别日志
 */
template <typename... Args>
void LogWarn(
    std::string_view filterType,
    Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    LogMessageCore(filterType, LogLevel::LOG_LEVEL_WARN, message, std::forward<Args>(args)...);
}

/**
 * @brief 输出 ERROR 级别日志
 */
template <typename... Args>
void LogError(
    std::string_view filterType,
    Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    LogMessageCore(filterType, LogLevel::LOG_LEVEL_ERROR, message, std::forward<Args>(args)...);
}

/**
 * @brief 输出 FATAL 级别日志
 */
template <typename... Args>
void LogFatal(
    std::string_view filterType,
    Utils::FormatString<Args...> message,
    Args&&... args) noexcept
{
    LogMessageCore(filterType, LogLevel::LOG_LEVEL_FATAL, message, std::forward<Args>(args)...);
}

END_NAMESPACE_COMMON

// ============================================================================
// 向后兼容的宏定义（可选，用于逐步迁移）
// ============================================================================

#define RC_LOG_TRACE(filterType__, message__, ...) LogTrace(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_DEBUG(filterType__, message__, ...) LogDebug(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_INFO(filterType__, message__, ...)  LogInfo(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_WARN(filterType__, message__, ...)  LogWarn(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_ERROR(filterType__, message__, ...) LogError(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_FATAL(filterType__, message__, ...) LogFatal(filterType__, message__, ## __VA_ARGS__)

#endif // RENDU_LOG_H
