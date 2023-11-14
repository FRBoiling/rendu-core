/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_TIMER_H
#define RENDU_COMMON_TIMER_H

#include "base/atomic/atomic.h"
#include "base/utils/non_copyable.h"
#include "timestamp.h"

COMMON_NAMESPACE_BEGIN
///
/// Internal class for timer event.
///
  typedef std::function<void()> TimerCallback;

  class Timer : NonCopyable {
  public:
    Timer(TimerCallback cb, Timestamp when, double interval)
      : callback_(std::move(cb)),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_.incrementAndGet()) {}

    void run() const {
      callback_();
    }

    Timestamp expiration() const { return expiration_; }

    bool repeat() const { return repeat_; }

    INT64 sequence() const { return sequence_; }

    void restart(Timestamp now);

    static INT64 numCreated() { return s_numCreated_.get(); }

  private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const INT64 sequence_;

    static AtomicInt64 s_numCreated_;
  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_TIMER_H
