/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_TIME_ID_H
#define RENDU_TIME_ID_H

#include "base/copyable.h"

RD_NAMESPACE_BEGIN

  class Timer;

///
/// An opaque identifier, for canceling Timer.
///
  class TimerId : public Copyable {
  public:
    TimerId()
      : timer_(NULL),
        sequence_(0) {
    }

    TimerId(Timer *timer, int64_t seq)
      : timer_(timer),
        sequence_(seq) {
    }

    // default copy-ctor, dtor and assignment are okay

    friend class TimerQueue;

  private:
    Timer *timer_;
    int64_t sequence_;
  };

RD_NAMESPACE_END

#endif //RENDU_TIME_ID_H
