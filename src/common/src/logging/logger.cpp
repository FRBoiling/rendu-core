#include "common/logging/logger.h"
#include "common/logging/log_message.h"
#include "common/logging/appender.h"
#include <utility>
#include <vector>

BEGIN_NAMESPACE_COMMON
using namespace Logging;

Logger::Logger(std::string _name, LogLevel _level) : _name(std::move(_name)), _level(_level)
{
}

std::string const& Logger::getName() const
{
    return _name;
}

LogLevel Logger::getLogLevel() const
{
    return _level;
}

void Logger::addAppender(Appender* appender)
{
    _appenders.push_back(appender);
}

void Logger::setLogLevel(LogLevel _level)
{
    _level = _level;
}

void Logger::write(LogMessage* message) const
{
    if (!_level || _level > message->level || message->text.empty())
    {
        //fprintf(stderr, "Logger::write: Logger %s, Level %u. Msg %s Level %u WRONG LEVEL MASK OR EMPTY MSG\n", getName().c_str(), getLogLevel(), message.text.c_str(), message.level);
        return;
    }

    for (Appender* appender : _appenders)
        appender->write(message);
}

END_NAMESPACE_COMMON