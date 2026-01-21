//
// Created by 沸腾 on 2025/11/3.
//


#include "common/logger/log.h"
#include "common/logger/appender_console.h"
#include "common/logger/log_message.h"
#include "common/logger/log_operation.h"
#include "common/utils/string_convert.h"
#include "common/utils/string_utils.h"
#include "common/utils/time_utils.h"
#include "common/asio/io_context_strand.h"
#include "common/asio/post.h"

BEGIN_NAMESPACE_COMMON
    Log::Log() : m_appenderId(0), m_lowestLogLevel(LogLevel::LOG_LEVEL_FATAL), m_logsTimestamp('_' + getTimestampStr())
    {
    }

    Log::~Log()
    {
        close();
    }


    Log* Log::instance() noexcept
    {
        static Log instance;
        return &instance;
    }

    void Log::initialize(Asio::IoContext* ioContext)
    {
        if (ioContext)
        {
            m_ioContext = ioContext;
            m_strand = std::make_unique<Asio::IoContext::Strand>(*ioContext);
        }

        // LoadFromConfig();
    }


    void Log::close()
    {
        m_loggers.clear();
        m_appenders.clear();
    }

    bool Log::setLogLevel(std::string const& name, int32 newLeveli, bool isLogger /* = true */)
    {
        if (newLeveli < 0)
            return false;

        auto newLevel = static_cast<LogLevel>(newLeveli);

        if (isLogger)
        {
            auto it = m_loggers.begin();
            while (it != m_loggers.end() && it->second->getName() != name)
                ++it;

            if (it == m_loggers.end())
                return false;

            it->second->setLogLevel(newLevel);

            if (newLevel != LogLevel::LOG_LEVEL_DISABLED && newLevel < m_lowestLogLevel)
                m_lowestLogLevel = newLevel;
        }
        else
        {
            Appender* appender = getAppenderByName(name);
            if (!appender)
                return false;

            appender->setLogLevel(newLevel);
        }

        return true;
    }


    uint8 Log::nextAppenderId()
    {
        return m_appenderId++;
    }

    Appender* Log::getAppenderByName(std::string_view name)
    {
        auto it = m_appenders.begin();
        while (it != m_appenders.end() && it->second && it->second->getName() != name)
            ++it;

        return it == m_appenders.end() ? nullptr : it->second.get();
    }

    void Log::registerAppender(uint8 index, AppenderCreatorFn appenderCreateFn)
    {
        [[maybe_unused]] bool isNewAppender = m_appenderFactory.try_emplace(index, appenderCreateFn).second;
        ASSERT(isNewAppender);
    }

    void Log::outMessageImpl(Logger const* logger, std::string_view filter, LogLevel level,
                             Utils::FormatStringView messageFormat, Utils::FormatArgs messageFormatArgs) const noexcept
    {
        if (m_ioContext)
            Asio::Post(*m_strand, LogOperation(
                           logger, std::make_unique<LogMessage>(level, filter,
                                                                Utils::StringVFormat(messageFormat, messageFormatArgs)).
                           get()));
        else
        {
            LogMessage msg(level, filter, Utils::StringVFormat(messageFormat, messageFormatArgs));
            logger->write(&msg);
        }
    }

    // 获取对应类型的日志器（支持层级查找，如type.sub1.sub2会查找type.sub1、type，最后root）
    Logger const* Log::getLoggerByType(std::string_view type) const
    {
        if (const auto it = m_loggers.find(type); it != m_loggers.end())
            return it->second.get();

        if (type == LOGGER_ROOT)
            return nullptr;

        std::string_view parentLogger = LOGGER_ROOT;
        size_t found = type.find_last_of('.');
        if (found != std::string::npos)
            parentLogger = type.substr(0, found);

        return getLoggerByType(parentLogger);
    }

    std::string Log::getTimestampStr()
    {
        return Utils::TimeToTimestampStr(time(nullptr));
    }


    bool Log::shouldLog(std::string_view type, LogLevel level) const noexcept
    {
        // 快速检查：如果请求的日志级别低于所有日志器的最低级别，直接返回false
        if (level < m_lowestLogLevel)
            return false;

        Logger const* logger = getLoggerByType(type);
        if (!logger)
            return false;

        // 检查日志器的日志级别配置
        LogLevel logLevel = logger->getLogLevel();
        return logLevel != LogLevel::LOG_LEVEL_DISABLED && logLevel <= level;
    }

    Logger const* Log::getEnabledLogger(std::string_view type, LogLevel level) const noexcept
    {
        // 快速检查：如果请求的日志级别低于所有日志器的最低级别，直接返回nullptr
        if (level < m_lowestLogLevel)
            return nullptr;

        Logger const* logger = getLoggerByType(type);
        if (!logger)
            return nullptr;

        LogLevel logLevel = logger->getLogLevel();
        return logLevel != LogLevel::LOG_LEVEL_DISABLED && logLevel <= level ? logger : nullptr;
    }


    void Log::createAppenderFromConfigLine(std::string const& appenderName, std::string const& options)
    {
        if (appenderName.empty())
            return;

        // Format = type, level, flags, optional1, optional2
        // if type = File. optional1 = file and option2 = mode
        // if type = Console. optional1 = Color

        std::vector<std::string_view> tokens = Utils::Tokenize(options, ',', true);

        size_t const size = tokens.size();
        std::string name = appenderName.substr(9);

        if (size < 2)
        {
            fprintf(stderr, "Log::CreateAppenderFromConfig: Wrong configuration for appender %s. Config line: %s\n",
                    name.c_str(), options.c_str());
            return;
        }

        AppenderFlags flags = AppenderFlags::APPENDER_FLAGS_NONE;

        auto type = Utils::StringToEnum<AppenderType>(AppenderType::APPENDER_INVALID, tokens[0]);
        auto level = Utils::StringToEnum<LogLevel>(LogLevel::LOG_LEVEL_INVALID, tokens[1]);

        auto factoryFunction = m_appenderFactory.find(static_cast<uint8>(type));
        if (factoryFunction == m_appenderFactory.end())
        {
            fprintf(stderr, "Log::CreateAppenderFromConfig: Unknown type '" STRING_VIEW_FMT "' for appender %s\n",
                    STRING_VIEW_FMT_ARG(tokens[0]), name.c_str());
            return;
        }

        if (level > LogLevel::NUM_ENABLED_LOG_LEVELS)
        {
            fprintf(stderr, "Log::CreateAppenderFromConfig: Wrong Log Level '" STRING_VIEW_FMT "' for appender %s\n",
                    STRING_VIEW_FMT_ARG(tokens[1]), name.c_str());
            return;
        }

        if (size > 2)
        {
            if (Optional<uint8> flagsVal = Utils::StringTo<uint8>(tokens[2]))
                flags = static_cast<AppenderFlags>(*flagsVal);
            else
            {
                fprintf(stderr, "Log::CreateAppenderFromConfig: Unknown flags '" STRING_VIEW_FMT "' for appender %s\n",
                        STRING_VIEW_FMT_ARG(tokens[2]), name.c_str());
                return;
            }
        }

        try
        {
            auto id = nextAppenderId();
            m_appenders.try_emplace(id,
                                    factoryFunction->second(id, std::move(name), level, flags, tokens)
            );
        }
        catch (InvalidAppenderArgsException const& iaae)
        {
            fprintf(stderr, "%s\n", iaae.what());
        }
    }

    void Log::createLoggerFromConfigLine(std::string const& loggerName, std::string const& options)
    {
        if (loggerName.empty())
            return;

        LogLevel level = LogLevel::LOG_LEVEL_DISABLED;

        std::string name = loggerName.substr(7);

        if (options.empty())
        {
            fprintf(stderr, "Log::CreateLoggerFromConfig: Missing config option Logger.%s\n", name.c_str());
            return;
        }

        std::vector<std::string_view> tokens = Utils::Tokenize(options, ',', true);

        if (tokens.size() != 2)
        {
            fprintf(stderr, "Log::CreateLoggerFromConfig: Wrong config option Logger.%s=%s\n", name.c_str(),
                    options.c_str());
            return;
        }

        if (m_loggers.contains(name))
        {
            fprintf(stderr, "Error while configuring Logger %s. Already defined\n", name.c_str());
            return;
        }

        level = Utils::StringToEnum<LogLevel>(LogLevel::LOG_LEVEL_INVALID, tokens[0]);

        if (level > LogLevel::NUM_ENABLED_LOG_LEVELS)
        {
            fprintf(stderr, "Log::CreateLoggerFromConfig: Wrong Log Level '" STRING_VIEW_FMT "' for logger %s\n",
                    STRING_VIEW_FMT_ARG(tokens[0]), name.c_str());
            return;
        }

        if (level < m_lowestLogLevel)
            m_lowestLogLevel = level;

        m_loggers[name] = std::make_unique<Logger>(name, level);
        Logger* logger = m_loggers[name].get();

        for (std::string_view appenderName : Utils::Tokenize(tokens[1], ' ', false))
        {
            if (Appender* appender = getAppenderByName(appenderName))
            {
                logger->addAppender(appender);
            }
            else
                fprintf(stderr,
                        "Error while configuring Appender " STRING_VIEW_FMT " in Logger %s. Appender does not exist\n",
                        STRING_VIEW_FMT_ARG(appenderName), name.c_str());
        }
    }

    void Log::setSynchronous()
    {
        m_strand.reset();
        m_ioContext = nullptr;
    }


END_NAMESPACE_COMMON
