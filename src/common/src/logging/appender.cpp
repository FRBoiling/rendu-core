//
// Created by 沸腾 on 2025/11/7.
//

#include "common/logging/appender.h"
#include "common/logging/log_message.h"
#include <string_view>

using namespace Rendu::Logging;
using namespace std::literals::string_view_literals;

Appender::Appender(uint8 _id, std::string _name, LogLevel _level /* = LOG_LEVEL_DISABLED */, AppenderFlags _flags /* = APPENDER_FLAGS_NONE */):
id(_id), name(std::move(_name)), level(_level), flags(_flags) { }

Appender::~Appender() = default;

uint8 Appender::getId() const
{
    return id;
}

std::string const& Appender::getName() const
{
    return name;
}

LogLevel Appender::getLogLevel() const
{
    return level;
}

AppenderFlags Appender::getFlags() const
{
    return flags;
}

void Appender::setLogLevel(LogLevel _level)
{
    level = _level;
}

void Appender::write(LogMessage* message)
{
    if (!level || level > message->level)
        return;

    if (flags & (APPENDER_FLAGS_PREFIX_TIMESTAMP | APPENDER_FLAGS_PREFIX_LOGLEVEL | APPENDER_FLAGS_PREFIX_LOGFILTERTYPE))
    {
        message->prefix.reserve(100);
        message->prefix.clear();

        if (flags & APPENDER_FLAGS_PREFIX_TIMESTAMP)
        {
            message->prefix.append(message->getTimeStr());
            message->prefix.append(1, ' ');
        }

        if (flags & APPENDER_FLAGS_PREFIX_LOGLEVEL)
        {
            std::string_view levelStr = getLogLevelString(message->level);
            message->prefix.append(levelStr);
            if (levelStr.length() < 5)
                message->prefix.append(5 - levelStr.length(), ' ');
        }

        if (flags & APPENDER_FLAGS_PREFIX_LOGFILTERTYPE)
        {
            message->prefix.append(1, '[');
            message->prefix.append(message->type);
            message->prefix.append("] ", 2);
        }
    }

    _write(message);
}

std::string_view Appender::getLogLevelString(LogLevel level)
{
    switch (level)
    {
        case LOG_LEVEL_FATAL:
            return "FATAL"sv;
        case LOG_LEVEL_ERROR:
            return "ERROR"sv;
        case LOG_LEVEL_WARN:
            return "WARN"sv;
        case LOG_LEVEL_INFO:
            return "INFO"sv;
        case LOG_LEVEL_DEBUG:
            return "DEBUG"sv;
        case LOG_LEVEL_TRACE:
            return "TRACE"sv;
        default:
            return "DISABLED"sv;
    }
}