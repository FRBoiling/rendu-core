/*
* Created by boil on 2024/7/13.
*/

#ifndef RENDU_TIME_TIME_TIME_CAST_H_
#define RENDU_TIME_TIME_TIME_CAST_H_

#include "time_duration.h"

RD_TIME_NAMESPACE_BEGIN

template<class _ToDuration, class _FromDuration>
_ToDuration DurationCast(const _FromDuration &__d) {
  return std::chrono::duration_cast<_ToDuration>(__d);
}

template<class _ToDuration>
inline _ToDuration DurationCast(const detail::SysTimePoint &__t) {
  return DurationCast<_ToDuration>(__t.time_since_epoch());
}


RD_TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_TIME_CAST_H_
