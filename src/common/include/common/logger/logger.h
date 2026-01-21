//
// Created by 沸腾 on 2025/10/31.
//

// ============================================================================
// logger.h - 日志器类
// ============================================================================

#ifndef RENDU_I_LOGGER_H
#define RENDU_I_LOGGER_H

#pragma once

#include <string>
#include <vector>
#include "common/define.h"
#include "log_level.h"

BEGIN_NAMESPACE_COMMON

// 前置声明
class Appender;
struct LogMessage;

// ============================================================================
// Logger - 日志器
// 管理多个追加器，按日志级别输出日志
// ============================================================================

class RC_COMMON_API Logger
{
public:
    /**
     * @brief 构造函数
     * @param name 日志器名称
     * @param level 最低日志级别
     */
    Logger(std::string name, LogLevel level);

    /**
     * @brief 添加追加器
     * @param appender 追加器指针
     */
    void addAppender(Appender* appender);

    /**
     * @brief 获取日志器名称
     * @return 日志器名称引用
     */
    std::string const& getName() const;

    /**
     * @brief 获取最低日志级别
     * @return 日志级别
     */
    LogLevel getLogLevel() const;

    /**
     * @brief 设置最低日志级别
     * @param level 新的日志级别
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief 写入日志消息
     * @param message 日志消息指针
     */
    void write(LogMessage* message) const;

private:
    std::string m_name;                  ///< 日志器名称
    LogLevel m_level;                    ///< 最低日志级别
    std::vector<Appender*> m_appenders;  ///< 追加器列表
};

END_NAMESPACE_COMMON

#endif // RENDU_I_LOGGER_H
