//
// Created by 沸腾 on 2025/11/7.
//

#ifndef RENDU_LOG_MESSAGE_H
#define RENDU_LOG_MESSAGE_H

#include "log_level.h"
#include <sstream>

BEGIN_NAMESPACE_COMMON
    namespace Logging
    {
        struct RC_COMMON_API LogMessage
        {
            LogMessage(LogLevel _level, std::string_view _type, std::string _text);
            LogMessage(LogLevel _level, std::string_view _type, std::string _text, std::string _param1);

            LogMessage(LogMessage const& /*other*/) = delete;
            LogMessage& operator=(LogMessage const& /*other*/) = delete;

            static std::string getTimeStr(time_t time);
            std::string getTimeStr() const;

            LogLevel const level;
            std::string const type;
            std::string const text;
            std::string prefix;
            std::string param1;
            time_t mtime;

            ///@ Returns size of the log message content in bytes
            uint32 Size() const
            {
                return static_cast<uint32>(prefix.size() + text.size());
            }
        };

    } // namespace logging

END_NAMESPACE_COMMON

#endif //RENDU_LOG_MESSAGE_H
