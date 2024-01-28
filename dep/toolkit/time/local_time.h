/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOCAL_TIME_H
#define RENDU_LOCAL_TIME_H

#include <sys/time.h>
#include "define.h"

RD_NAMESPACE_BEGIN

  void NoLocksLocalTime(struct tm *tmp, time_t t);

  void LocalTimeInit();

  int GetDaylightActive();

RD_NAMESPACE_END

#endif //RENDU_LOCAL_TIME_H
