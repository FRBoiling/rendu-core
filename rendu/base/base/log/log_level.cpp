/*
* Created by boil on 2023/10/26.
*/

#include "log_level.h"

RD_NAMESPACE_BEGIN

namespace LogLevel {

  const char *LogLevelNameTable[LogLevel::Level::LNum_LOG_LEVELS][2] = {
      {"TRACE ", "T"},
      {"DEBUG ", "D"},
      {"INFO ", "I"},
      {"WARN ", "W"},
      {"ERROR ", "E"},
      {"Critical ", "C"},
  };


  const char *GetLogLevelName(LogLevel::Level level) {
    return LogLevelNameTable[level][0];
  }

  const char *GetLogLevelSimpleName(LogLevel::Level level) {
    return LogLevelNameTable[level][1];
  }

}// namespace LogLevel

RD_NAMESPACE_END
