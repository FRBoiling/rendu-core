/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_LOG_LEVEL_H
#define RENDU_LOG_LEVEL_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  const char *GetLogLevelName(LogLevel level);

RD_NAMESPACE_END

#endif //RENDU_LOG_LEVEL_H
