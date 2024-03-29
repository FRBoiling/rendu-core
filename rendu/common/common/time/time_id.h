/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_TIME_ID_H
#define RENDU_COMMON_TIME_ID_H

#include "common_define.h"

COMMON_NAMESPACE_BEGIN

  class Timer;

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

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_TIME_ID_H
