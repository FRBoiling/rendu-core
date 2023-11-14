/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_LOG_LEVEL_H
#define RENDU_BASE_LOG_LEVEL_H

#include "log_define.h"

RD_NAMESPACE_BEGIN

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

  extern const char *GetLogLevelName(LogLevel::Level level);

  extern const char *GetLogLevelSimpleName(LogLevel::Level level);

}// namespace LogLevel

RD_NAMESPACE_END

#endif//RENDU_BASE_LOG_LEVEL_H
