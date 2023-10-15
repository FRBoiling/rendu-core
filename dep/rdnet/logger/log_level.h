/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOG_LEVEL_H
#define RENDU_LOG_LEVEL_H

#include "define.h"

RD_NAMESPACE_BEGIN

  enum LogLevel {
    LTrace = 0, LDebug, LInfo, LWarn, LError
  };

RD_NAMESPACE_END

#endif //RENDU_LOG_LEVEL_H
