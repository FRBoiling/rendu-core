/*
* Created by boil on 2023/11/2.
*/

#include "timer.h"
#include "base/atomic.h"

RD_NAMESPACE_BEGIN

  AtomicInt64 Timer::s_numCreated_;

  void Timer::restart(Timestamp now) {
    if (repeat_) {
      expiration_ = addTime(now, interval_);
    } else {
      expiration_ = Timestamp::invalid();
    }
  }

RD_NAMESPACE_END