//
// Created by 沸腾 on 2025/11/7.
//

#include "common/logger/log_message.h"
#include "common/utils/string_format.h"

BEGIN_NAMESPACE_COMMON


LogMessage::LogMessage(LogLevel level, std::string_view type, std::string text)
    : m_level(level), m_type(type), m_text(std::move(text)), m_time(time(nullptr))
{
}

LogMessage::LogMessage(LogLevel level, std::string_view type, std::string text, std::string param1)
    : m_level(level), m_type(type), m_text(std::move(text)), m_param1(std::move(param1)), m_time(time(nullptr))
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
    return getTimeStr(m_time);
}

END_NAMESPACE_COMMON