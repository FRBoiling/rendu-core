/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOG_LEVEL_H
#define RENDU_LOG_LEVEL_H

#include "log_define.h"

LOG_NAMESPACE_BEGIN

namespace LogLevel {

  enum Level {
    LInvalid = -1,
    LTrace = 0,
    LDebug,
    LInfo,
    LWarn,
    LError,
    LCritical,
    LNum_LOG_LEVELS
  };

  static const char *LogLevelNameTable[LogLevel::Level::LNum_LOG_LEVELS][2] = {
      {"TRACE ", "T"},
      {"DEBUG ", "D"},
      {"INFO ", "I"},
      {"WARN ", "W"},
      {"ERROR ", "E"},
      {"Critical ", "C"},
  };

  static const char *GetLogLevelName(LogLevel::Level level) {
    return LogLevelNameTable[level][0];
  }

  static const char *GetLogLevelSimpleName(LogLevel::Level level) {
    return LogLevelNameTable[level][1];
  }

}// namespace LogLevel

LOG_NAMESPACE_END

#endif//RENDU_LOG_LEVEL_H
