/*
* Created by boil on 2023/12/21.
*/

#include "logger_enum.h"

COMMON_NAMESPACE_BEGIN

const char *LogLevelNameTable[LogLevel::LL_Num_LOG_LEVELS][2] = {
    {"TRACE", "T"},
    {"DEBUG", "D"},
    {"INFO", "I"},
    {"WARN", "W"},
    {"ERROR", "E"},
    {"Critical", "C"},
};


const char *GetLogLevelName(LogLevel level) {
  return LogLevelNameTable[level][0];
}

const char *GetLogLevelSimpleName(LogLevel level) {
  return LogLevelNameTable[level][1];
}

COMMON_NAMESPACE_END
