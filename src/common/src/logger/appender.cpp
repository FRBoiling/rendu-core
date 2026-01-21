//
// Created by 沸腾 on 2025/11/7.
//

#include "common/logger/appender.h"
#include "common/logger/log_level.h"
#include <string_view>

#include "common/logger/log_message.h"

BEGIN_NAMESPACE_COMMON
    using namespace std::literals::string_view_literals;

    Appender::Appender(uint8 id, std::string name, LogLevel level, AppenderFlags flags)
        : m_id(id), m_name(std::move(name)), m_level(level), m_flags(flags)
    {
    }

    Appender::~Appender() = default;

    uint8 Appender::getId() const
    {
        return m_id;
    }

    std::string const& Appender::getName() const
    {
        return m_name;
    }

    LogLevel Appender::getLogLevel() const
    {
        return m_level;
    }

    AppenderFlags Appender::getFlags() const
    {
        return m_flags;
    }

    void Appender::setLogLevel(LogLevel level)
    {
        m_level = level;
    }

    void Appender::write(LogMessage* message)
    {
        if (m_level == LogLevel::LOG_LEVEL_DISABLED || m_level > message->m_level)
            return;

        // 使用局部变量构造前缀，避免并发修改共享的 message->prefix
        std::string newPrefix;

        if (static_cast<bool>(m_flags & (AppenderFlags::APPENDER_FLAGS_PREFIX_TIMESTAMP |
            AppenderFlags::APPENDER_FLAGS_PREFIX_LOGLEVEL |
            AppenderFlags::APPENDER_FLAGS_PREFIX_LOGFILTERTYPE)))
        {
            newPrefix.reserve(64); // 从 100 减少到 64，足以容纳典型前缀长度

            if (static_cast<bool>(m_flags & AppenderFlags::APPENDER_FLAGS_PREFIX_TIMESTAMP))
            {
                newPrefix.append(message->getTimeStr());
                newPrefix.append(1, ' ');
            }

            if (static_cast<bool>(m_flags & AppenderFlags::APPENDER_FLAGS_PREFIX_LOGLEVEL))
            {
                std::string_view levelStr = getLogLevelString(message->m_level);
                newPrefix.append(levelStr);
                if (levelStr.length() < 5)
                    newPrefix.append(5 - levelStr.length(), ' ');
            }

            if (static_cast<bool>(m_flags & AppenderFlags::APPENDER_FLAGS_PREFIX_LOGFILTERTYPE))
            {
                newPrefix.append(1, '[');
                newPrefix.append(message->m_type);
                newPrefix.append("] ", 2);
            }
        }

        // 原子性赋值给 message->prefix，确保线程安全
        message->m_prefix = std::move(newPrefix);

        _write(message);
    }

    std::string_view Appender::getLogLevelString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::LOG_LEVEL_FATAL:
            return "FATAL"sv;
        case LogLevel::LOG_LEVEL_ERROR:
            return "ERROR"sv;
        case LogLevel::LOG_LEVEL_WARN:
            return "WARN"sv;
        case LogLevel::LOG_LEVEL_INFO:
            return "INFO"sv;
        case LogLevel::LOG_LEVEL_DEBUG:
            return "DEBUG"sv;
        case LogLevel::LOG_LEVEL_TRACE:
            return "TRACE"sv;
        default:
            return "DISABLED"sv;
        }
    }

END_NAMESPACE_COMMON
