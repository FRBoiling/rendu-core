//
// Created by 沸腾 on 2025/10/31.
//

#ifndef RENDU_I_LOGGER_H
#define RENDU_I_LOGGER_H

#include <string>
#include <vector>
#include "common/define.h"
#include "log_level.h"

BEGIN_NAMESPACE_COMMON
class Appender;
struct LogMessage;

class RC_COMMON_API Logger
{
public:
    Logger(std::string const& name, LogLevel level);

    void addAppender(Appender* appender);

    std::string const& getName() const;
    LogLevel getLogLevel() const;
    void setLogLevel(LogLevel level);
    void write(LogMessage* message) const;

private:
    std::string _name;
    LogLevel _level;
    std::vector<Appender*> _appenders;
};

END_NAMESPACE_COMMON

#endif //RENDU_I_LOGGER_H
