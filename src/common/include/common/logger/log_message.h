//
// Created by 沸腾 on 2025/11/7.
//
#pragma once
#ifndef RENDU_LOG_MESSAGE_H
#define RENDU_LOG_MESSAGE_H

#include "log_level.h"
#include <sstream>

BEGIN_NAMESPACE_COMMON

    struct RC_COMMON_API LogMessage
    {
        LogMessage(LogLevel level, std::string_view type, std::string text);
        LogMessage(LogLevel level, std::string_view type, std::string text, std::string param1);

        LogMessage(LogMessage const& /*other*/) = delete;
        LogMessage& operator=(LogMessage const& /*other*/) = delete;

        static std::string getTimeStr(time_t time);
        std::string getTimeStr() const;

        LogLevel const m_level;
        std::string const m_type;
        std::string const m_text;
        std::string m_prefix;
        std::string m_param1;
        time_t m_time;

        ///@ Returns size of the log message content in bytes
        uint32 size() const
        {
            return static_cast<uint32>(m_prefix.size() + m_text.size());
        }
    };

END_NAMESPACE_COMMON

#endif //RENDU_LOG_MESSAGE_H
