/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_CORE_CORE_WORLD_LOG_LOG_H_
#define RENDU_CORE_CORE_WORLD_LOG_LOG_H_

#include "core_define.h"

RD_NAMESPACE_BEGIN
class Log {
  public:
    static void Error(const Exception &exception);
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_WORLD_LOG_LOG_H_
