/*
* Created by boil on 2023/11/2.
*/

#include "timer.h"
#include "base/number/atomic.h"

COMMON_NAMESPACE_BEGIN

  AtomicInt64 Timer::s_numCreated_;

  void Timer::restart(Timestamp now) {
    if (repeat_) {
      expiration_ = addTime(now, interval_);
    } else {
      expiration_ = Timestamp::invalid();
    }
  }

COMMON_NAMESPACE_END