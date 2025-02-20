/*
* Created by boil on 2024/8/7.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOG_LEVEL_H_
#define RENDU_BASIC_BASIC_LOG_LOG_LEVEL_H_

#include "log_define.h"

RD_NAMESPACE_BEGIN

//日志输出等级
enum LogLevel : int {
  LL_OFF = -1,
  LL_TRACE = 0,
  LL_DEBUG = 1,
  LL_INFO = 2,
  LL_WARN = 3,
  LL_ERROR = 4,
  LL_CRITICAL = 5,
  LL_Num_LOG_LEVELS = 6,
};


extern const char *GetLogLevelName(LogLevel level);

extern const char *GetLogLevelSimpleName(LogLevel level);

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_LOG_LOG_LEVEL_H_
