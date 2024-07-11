/*
* Created by boil on 2024/7/13.
*/

#ifndef RENDU_TIME_TIME_TIME_FLOOR_H_
#define RENDU_TIME_TIME_TIME_FLOOR_H_

#include "time_cast.h"

RD_TIME_NAMESPACE_BEGIN


/**
     * rounds down
     */
template<class _ToDuration, class _FromDuration>
_ToDuration Floor(const _FromDuration &d) {
  _ToDuration t = DurationCast<_ToDuration>(d);
  if (t > d) --t;
  return t;
}


RD_TIME_NAMESPACE_END


#endif//RENDU_TIME_TIME_TIME_FLOOR_H_
