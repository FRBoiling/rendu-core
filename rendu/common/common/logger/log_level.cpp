/*
* Created by boil on 2023/11/2.
*/

#include "log_level.h"

RD_NAMESPACE_BEGIN

  const char *LogLevelName[LogLevel::NUM_LOG_LEVELS] = {
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
  };

  const char *GetLogLevelName(LogLevel level) {
    return LogLevelName[level];
  }

RD_NAMESPACE_END