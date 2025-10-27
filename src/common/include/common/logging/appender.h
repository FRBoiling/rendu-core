//
// Created by 沸腾 on 2025/11/7.
//

#ifndef RENDU_APPENDER_H
#define RENDU_APPENDER_H

#include <string>

#include "log_level.h"
#include "common/define.h"

BEGIN_NAMESPACE_COMMON

enum AppenderType : uint8
{
    APPENDER_NONE,
    APPENDER_CONSOLE,
    APPENDER_FILE,
    APPENDER_DB,

    APPENDER_INVALID = 0xFF // SKIP
};

enum AppenderFlags : uint8
{
    APPENDER_FLAGS_NONE                          = 0x00,
    APPENDER_FLAGS_PREFIX_TIMESTAMP              = 0x01,
    APPENDER_FLAGS_PREFIX_LOGLEVEL               = 0x02,
    APPENDER_FLAGS_PREFIX_LOGFILTERTYPE          = 0x04,
    APPENDER_FLAGS_USE_TIMESTAMP                 = 0x08, // only used by FileAppender
    APPENDER_FLAGS_MAKE_FILE_BACKUP              = 0x10  // only used by FileAppender
};

struct LogMessage;

class RC_COMMON_API Appender
{
public:
    Appender(uint8 _id, std::string name, LogLevel level = LOG_LEVEL_DISABLED, AppenderFlags flags = APPENDER_FLAGS_NONE);
    Appender(Appender const&) = delete;
    Appender(Appender&&) = delete;
    Appender& operator=(Appender const&) = delete;
    Appender& operator=(Appender&&) = delete;
    virtual ~Appender();

    uint8 getId() const;
    std::string const& getName() const;
    virtual AppenderType getType() const = 0;
    LogLevel getLogLevel() const;
    AppenderFlags getFlags() const;

    void setLogLevel(LogLevel);
    void write(LogMessage* message);
    static std::string_view getLogLevelString(LogLevel level);
    virtual void setRealmId(uint32 /*realmId*/) { }

private:
    virtual void _write(LogMessage const* /*message*/) = 0;

    uint8 id;
    std::string name;
    LogLevel level;
    AppenderFlags flags;
};

END_NAMESPACE_COMMON

#endif //RENDU_APPENDER_H