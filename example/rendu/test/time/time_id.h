/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_TIME_TIME_ID_H
#define RENDU_TIME_TIME_ID_H

#include "time_define.h"
#include "timer.h"

TIME_NAMESPACE_BEGIN

///
/// An opaque identifier, for canceling Timer.
///
class TimerId : public Copyable {
public:
  TimerId()
      : timer_(nullptr),
        sequence_(0) {
  }

  TimerId(Timer *timer, std::int64_t seq)
      : timer_(timer),
        sequence_(seq) {
  }

  // default copy-ctor, dtor and assignment are okay

  friend class TimerQueue;

private:
  Timer *timer_;
  std::int64_t sequence_;
};

TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_ID_H
