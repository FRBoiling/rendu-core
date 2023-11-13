/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_LOCAL_TIME_H
#define RENDU_BASE_LOCAL_TIME_H

#include "base_define.h"
#include <sys/time.h>

RD_NAMESPACE_BEGIN

  void NoLocksLocalTime(struct tm *tmp, time_t t);

  void LocalTimeInit();

  int GetDaylightActive();

RD_NAMESPACE_END

#endif //RENDU_BASE_LOCAL_TIME_H
