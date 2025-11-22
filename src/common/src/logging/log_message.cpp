//
// Created by 沸腾 on 2025/11/7.
//

#include "common/logging/log_message.h"
#include "common/utils/string_format.h"

BEGIN_NAMESPACE_COMMON

using namespace Logging;

LogMessage::LogMessage(LogLevel _level, std::string_view _type, std::string _text)
    : level(_level), type(_type), text(std::move(_text)), mtime(time(nullptr))
{
}

LogMessage::LogMessage(LogLevel _level, std::string_view _type, std::string _text, std::string _param1)
    : level(_level), type(_type), text(std::move(_text)), param1(std::move(_param1)), mtime(time(nullptr))
{
}

std::string LogMessage::getTimeStr(time_t time)
{
    tm aTm;
    localtime_r(&time, &aTm);
    return Utils::StringFormat("{:04}-{:02}-{:02}_{:02}:{:02}:{:02}", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday,
                                 aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
}

std::string LogMessage::getTimeStr() const
{
    return getTimeStr(mtime);
}

END_NAMESPACE_COMMON