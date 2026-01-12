//
// Created by 沸腾 on 2025/11/3.
//

#pragma once

#ifndef RENDU_LOG_H
#define RENDU_LOG_H

#include <unordered_map>
#include <memory>

#include "appender.h"
#include "logger.h"
#include "log_level.h"
#include "common/asio/io_context.h"
#include "common/utils/string_format.h"

BEGIN_NAMESPACE_COMMON
#define LOGGER_ROOT "root"

    class RC_COMMON_API Log
    {
    private:
        Log();
        ~Log();

    public:
        Log(Log const&) = delete;
        Log(Log&&) = delete;
        Log& operator=(Log const&) = delete;
        Log& operator=(Log&&) = delete;

        static Log* instance() noexcept;

        void initialize(Asio::IoContext* ioContext);
        void close();

        bool setLogLevel(std::string const& name, int32 level, bool isLogger = true);

        template <class AppenderImpl>
        void registerAppender()
        {
            this->registerAppender(AppenderImpl::type, &CreateAppender<AppenderImpl>);
        }

        template <typename... Args>
        void outMessageTo(Logger const* logger, std::string_view filter, LogLevel level,
                          Utils::FormatString<Args...> fmt,
                          Args&&... args) noexcept
        {
            this->outMessageImpl(logger, filter, level, fmt, Utils::MakeFormatArgs(args...));
        }


        [[nodiscard]] bool shouldLog(std::string_view type, LogLevel level) const noexcept;
        [[nodiscard]] Logger const* getEnabledLogger(std::string_view type, LogLevel level) const noexcept;


        [[nodiscard]] std::string const& getLogsDir() const { return m_logsDir; }
        [[nodiscard]] std::string const& getLogsTimestamp() const { return m_logsTimestamp; }

        void setSynchronous(); // Not threadsafe - should only be called from main() after all threads are joined

        void createAppenderFromConfigLine(std::string const& name, std::string const& options);
        void createLoggerFromConfigLine(std::string const& name, std::string const& options);

    private:
        static std::string getTimestampStr();

        [[nodiscard]] Logger const* getLoggerByType(std::string_view type) const;
        [[nodiscard]] Appender* getAppenderByName(std::string_view name);
        [[nodiscard]] uint8 nextAppenderId();

        void registerAppender(uint8 index, AppenderCreatorFn appenderCreateFn);
        void outMessageImpl(Logger const* logger, std::string_view filter, LogLevel level,
                            Utils::FormatStringView messageFormat,
                            Utils::FormatArgs messageFormatArgs) const noexcept;

        std::unordered_map<uint8, AppenderCreatorFn> m_appenderFactory;
        std::unordered_map<uint8, std::unique_ptr<Appender>> m_appenders;
        std::unordered_map<std::string_view, std::unique_ptr<Logger>> m_loggers;

        uint8 m_appenderId;
        LogLevel m_lowestLogLevel;

        std::string m_logsDir;
        std::string m_logsTimestamp;

        Asio::IoContext* m_ioContext = nullptr;
        std::unique_ptr<Asio::IoContext::Strand> m_strand;
    };

    template <typename... Args>
    void LogMessageCore(std::string_view filterType, LogLevel level,
                        Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        Log* logInstance = Log::instance();
        if (Logger const* loggerInstance = logInstance->getEnabledLogger(filterType, level))
        {
            logInstance->outMessageTo(loggerInstance, filterType, level,
                                      message, std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    void LogTrace(std::string_view filterType, Rendu::Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        LogMessageCore(filterType, LogLevel::LOG_LEVEL_TRACE, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void LogDebug(std::string_view filterType, Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        LogMessageCore(filterType, LogLevel::LOG_LEVEL_DEBUG, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void LogInfo(std::string_view filterType, Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        LogMessageCore(filterType, LogLevel::LOG_LEVEL_INFO, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void LogWarn(std::string_view filterType, Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        LogMessageCore(filterType, LogLevel::LOG_LEVEL_WARN, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void LogError(std::string_view filterType, Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        LogMessageCore(filterType, LogLevel::LOG_LEVEL_ERROR, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void LogFatal(std::string_view filterType, Utils::FormatString<Args...> message, Args&&... args) noexcept
    {
        LogMessageCore(filterType, LogLevel::LOG_LEVEL_FATAL, message, std::forward<Args>(args)...);
    }

END_NAMESPACE_COMMON

// 向后兼容的宏定义（可选，用于逐步迁移）
#define RC_LOG_TRACE(filterType__, message__, ...) LogTrace(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_DEBUG(filterType__, message__, ...) LogDebug(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_INFO(filterType__, message__, ...)  LogInfo(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_WARN(filterType__, message__, ...)  LogWarn(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_ERROR(filterType__, message__, ...) LogError(filterType__, message__, ## __VA_ARGS__)
#define RC_LOG_FATAL(filterType__, message__, ...) LogFatal(filterType__, message__, ## __VA_ARGS__)


#endif //RENDU_LOG_H
