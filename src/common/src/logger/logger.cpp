#include "common/logger/logger.h"
#include "common/logger/log_message.h"
#include "common/logger/appender.h"
#include <utility>
#include <vector>

BEGIN_NAMESPACE_COMMON

Logger::Logger(std::string name, LogLevel level) : m_name(std::move(name)), m_level(level)
{
}

std::string const& Logger::getName() const
{
    return m_name;
}

LogLevel Logger::getLogLevel() const
{
    return m_level;
}

void Logger::addAppender(Appender* appender)
{
    m_appenders.push_back(appender);
}

void Logger::setLogLevel(LogLevel level)
{
    m_level = level;
}

void Logger::write(LogMessage* message) const
{
    if (m_level != LogLevel::LOG_LEVEL_DISABLED || m_level > message->m_level || message->m_text.empty())
    {
        //fprintf(stderr, "Logger::write: Logger %s, Level %u. Msg %s Level %u WRONG LEVEL MASK OR EMPTY MSG\n", getName().c_str(), getLogLevel(), message->text.c_str(), message->m_level);
        return;
    }

    for (Appender* appender : m_appenders)
        appender->write(message);
}

END_NAMESPACE_COMMON