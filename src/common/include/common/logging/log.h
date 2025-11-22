//
// Created by 沸腾 on 2025/11/3.
//

#ifndef RENDU_LOG_H
#define RENDU_LOG_H

#include "appender.h"
#include "logger.h"
#include "log_level.h"
#include "common/define.h"
#include "common/asio/io_context.h"
#include "common/asio/strand.h"
#include "common/utils/string_format.h"

BEGIN_NAMESPACE_COMMON
    namespace Logging
    {
#define LOGGER_ROOT "root"
        typedef Appender*(*AppenderCreatorFn)(uint8 id, std::string name, LogLevel level, AppenderFlags flags,
                                              std::vector<std::string_view> const& extraArgs);

        template <class AppenderImpl>
        Appender* CreateAppender(uint8 id, std::string name, LogLevel level, AppenderFlags flags,
                                 std::vector<std::string_view> const& extraArgs)
        {
            return new AppenderImpl(id, std::move(name), level, flags, extraArgs);
        }


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

            void Initialize(Asio::IoContext* ioContext);
            void SetSynchronous(); // Not threadsafe - should only be called from main() after all threads are joined
            void LoadFromConfig();
            void Close();
            bool ShouldLog(std::string_view type, LogLevel level) const noexcept;
            Logger const* GetEnabledLogger(std::string_view type, LogLevel level) const noexcept;
            bool SetLogLevel(std::string const& name, int32 level, bool isLogger = true);

            template <typename... Args>
            void OutMessage(std::string_view filter, LogLevel level, Utils::FormatString<Args...> fmt,
                            Args&&... args) noexcept
            {
                this->OutMessageImpl(GetLoggerByType(filter), filter, level, fmt, Utils::MakeFormatArgs(args...));
            }

            template <typename... Args>
            void OutMessageTo(Logger const* logger, std::string_view filter, LogLevel level,
                              Utils::FormatString<Args...> fmt,
                              Args&&... args) noexcept
            {
                this->OutMessageImpl(logger, filter, level, fmt, Utils::MakeFormatArgs(args...));
            }

            template <typename... Args>
            void OutCommand(uint32 account, Utils::FormatString<Args...> fmt, Args&&... args) noexcept
            {
                if (!ShouldLog("commands.gm", LOG_LEVEL_INFO))
                    return;

                this->OutCommandImpl(account, fmt, Utils::MakeFormatArgs(args...));
            }

            void OutCharDump(std::string const& str, uint32 account_id, uint64 guid,
                             std::string const& name) const noexcept;

            void SetRealmId(uint32 id);

            template <class AppenderImpl>
            void RegisterAppender()
            {
                this->RegisterAppender(AppenderImpl::type, &CreateAppender<AppenderImpl>);
            }

            std::string const& GetLogsDir() const { return m_logsDir; }
            std::string const& GetLogsTimestamp() const { return m_logsTimestamp; }

            void CreateAppenderFromConfigLine(std::string const& name, std::string const& options);
            void CreateLoggerFromConfigLine(std::string const& name, std::string const& options);

            template <typename StringOrStringView>
            static constexpr std::string_view make_string_view(StringOrStringView const& stringOrStringView)
            {
                return stringOrStringView;
            }

            template <size_t CharArraySize>
            static consteval std::string_view make_string_view(char const (&chars)[CharArraySize])
            {
                return {std::begin(chars), (chars[CharArraySize - 1] == '\0' ? CharArraySize - 1 : CharArraySize)};
            }

            template <size_t CharArraySize>
            static consteval Utils::FormatStringView make_format_string_view(char const (&chars)[CharArraySize])
            {
                return {std::begin(chars), (chars[CharArraySize - 1] == '\0' ? CharArraySize - 1 : CharArraySize)};
            }

        private:
            static std::string GetTimestampStr();

            Logger const* GetLoggerByType(std::string_view type) const;
            Appender* GetAppenderByName(std::string_view name);
            uint8 NextAppenderId();
            // void CreateAppenderFromConfig(std::string const& name);
            // void CreateLoggerFromConfig(std::string const& name);
            // void ReadAppendersFromConfig();
            // void ReadLoggersFromConfig();
            void RegisterAppender(uint8 index, AppenderCreatorFn appenderCreateFn);
            void OutMessageImpl(Logger const* logger, std::string_view filter, LogLevel level,
                                Utils::FormatStringView messageFormat,
                                Utils::FormatArgs messageFormatArgs) const noexcept;
            void OutCommandImpl(uint32 account, Utils::FormatStringView messageFormat,
                                Utils::FormatArgs messageFormatArgs) const noexcept;

            std::unordered_map<uint8, AppenderCreatorFn> appenderFactory;
            std::unordered_map<uint8, std::unique_ptr<Appender>> appenders;
            std::unordered_map<std::string_view, std::unique_ptr<Logger>> loggers;
            uint8 AppenderId;
            LogLevel lowestLogLevel;

            std::string m_logsDir;
            std::string m_logsTimestamp;

            Asio::IoContext* _ioContext;
            Asio::Strand* _strand;
        };

    } // namespace Logging



#define sLog Logging::Log::instance()

#define RC_LOG_MESSAGE_BODY_CORE(filterType__, level__, message__, ...)                                                         \
        do {                                                                                                                    \
            Logging::Log* logInstance = sLog;                                                                                            \
            if (Logging::Logger const* loggerInstance = logInstance->GetEnabledLogger(Logging::Log::make_string_view((filterType__)), (level__))) \
                logInstance->OutMessageTo(loggerInstance, Logging::Log::make_string_view((filterType__)), (level__),                     \
                    Logging::Log::make_format_string_view((message__)), ## __VA_ARGS__);                                                 \
        } while (0)

#ifdef PERFORMANCE_PROFILING
#define RC_LOG_MESSAGE_BODY(filterType__, level__, message__, ...) ((void)0)
#elif RENDU_PLATFORM != RENDU_PLATFORM_WINDOWS
#define RC_LOG_MESSAGE_BODY(filterType__, level__, message__, ...) RC_LOG_MESSAGE_BODY_CORE(filterType__, level__, message__, ## __VA_ARGS__)
#else
#define RC_LOG_MESSAGE_BODY(filterType__, level__, message__, ...)                 \
        __pragma(warning(push))                                                    \
        __pragma(warning(disable:4127))                                            \
        RC_LOG_MESSAGE_BODY_CORE(filterType__, level__, message__, ## __VA_ARGS__) \
        __pragma(warning(pop))
#endif

#define RC_DEFAULT_FILTER_TYPE "rendu"

#define RC_LOG_TRACE(filterType__, message__, ...) \
    RC_LOG_MESSAGE_BODY(filterType__, Logging::LOG_LEVEL_TRACE, message__, ## __VA_ARGS__)

#define RC_LOG_DEBUG(filterType__, message__, ...) \
    RC_LOG_MESSAGE_BODY(filterType__, Logging::LOG_LEVEL_DEBUG, message__, ## __VA_ARGS__)

#define RC_LOG_INFO(filterType__, message__, ...)  \
    RC_LOG_MESSAGE_BODY(filterType__, Logging::LOG_LEVEL_INFO, message__, ## __VA_ARGS__)

#define RC_LOG_WARN(filterType__, message__, ...)  \
    RC_LOG_MESSAGE_BODY(filterType__, Logging::LOG_LEVEL_WARN, message__, ## __VA_ARGS__)

#define RC_LOG_ERROR(filterType__, message__, ...) \
    RC_LOG_MESSAGE_BODY(filterType__, Logging::LOG_LEVEL_ERROR, message__, ## __VA_ARGS__)

#define RC_LOG_FATAL(filterType__, message__, ...) \
    RC_LOG_MESSAGE_BODY(filterType__, Logging::LOG_LEVEL_FATAL, message__, ## __VA_ARGS__)


END_NAMESPACE_COMMON

#endif //RENDU_LOG_H
