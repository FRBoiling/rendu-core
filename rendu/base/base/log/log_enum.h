/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_BASE_LOGGER_ENUM_H
#define RENDU_BASE_LOGGER_ENUM_H

#include "base_define.h"

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

//日志输出位置
enum LogPosition {
  LP_CONSOLE = 0x01,         //控制台
  LP_FILE = 0X02,            //文件
  LP_CONSOLE_AND_FILE = 0x03,//控制台+文件
};

enum LogMode {
  LM_SYNC, //同步模式
  LM_ASYNC,//异步模式
};

RD_NAMESPACE_END

#endif//RENDU_BASE_LOGGER_ENUM_H
