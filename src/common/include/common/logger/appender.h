//
// Created by 沸腾 on 2025/11/7.
//
#pragma once

#ifndef RENDU_APPENDER_H
#define RENDU_APPENDER_H


#include "common/define.h"
#include <string>
#include <vector>
#include "appender_flags.h"
#include "appender_type.h"
#include "log_level.h"

BEGIN_NAMESPACE_COMMON
    struct LogMessage;

    class RC_COMMON_API Appender
    {
    public:
        Appender(uint8 id, std::string name, LogLevel level = LogLevel::LOG_LEVEL_DISABLED, AppenderFlags flags = AppenderFlags::APPENDER_FLAGS_NONE);
        Appender(Appender const&) = delete;
        Appender(Appender&&) = delete;
        Appender& operator=(Appender const&) = delete;
        Appender& operator=(Appender&&) = delete;
        virtual ~Appender();

        [[nodiscard]] uint8 getId() const;
        [[nodiscard]] std::string const& getName() const;

        [[nodiscard]] virtual AppenderType getType() const = 0;
        [[nodiscard]] LogLevel getLogLevel() const;
        [[nodiscard]] AppenderFlags getFlags() const;

        void setLogLevel(LogLevel);
        void write(LogMessage* message);
        static std::string_view getLogLevelString(LogLevel level);
        virtual void setRealmId(uint32 /*realmId*/) { }

    private:
        virtual void _write(LogMessage const* /*message*/) = 0;

        uint8 m_id;
        std::string m_name;
        LogLevel m_level;
        AppenderFlags m_flags;
    };

    class RC_COMMON_API InvalidAppenderArgsException : public std::length_error
    {
    public:
        explicit InvalidAppenderArgsException(std::string const& message) : std::length_error(message)
        {
        }
    };


    typedef Appender*(*AppenderCreatorFn)(uint8 id, std::string name, LogLevel level, AppenderFlags flags,
                                          std::vector<std::string_view> const& extraArgs);

    template <class AppenderImpl>
    Appender* CreateAppender(uint8 id, std::string name, LogLevel level, AppenderFlags flags,
                             std::vector<std::string_view> const& extraArgs)
    {
        return new AppenderImpl(id, std::move(name), level, flags, extraArgs);
    }


END_NAMESPACE_COMMON

#endif //RENDU_APPENDER_H
