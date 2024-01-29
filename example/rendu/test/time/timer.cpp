/*
* Created by boil on 2023/11/2.
*/

#include "timer.h"
#include "base/atomic/atomic.h"

TIME_NAMESPACE_BEGIN

  std::atomic<INT64> Timer::s_numCreated_;

  void Timer::restart(Timestamp now) {
    if (repeat_) {
      expiration_ = addTime(now, interval_);
    } else {
      expiration_ = Timestamp::invalid();
    }
  }

TIME_NAMESPACE_END