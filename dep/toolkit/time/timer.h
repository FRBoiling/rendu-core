/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_TIMER_H
#define RENDU_TIMER_H

#include "utils/atomic.h"
#include "timestamp.h"
#include "event_loop.h"

RD_NAMESPACE_BEGIN
///
/// Internal class for timer event.
///

  class Timer {

  public:
    using Ptr = std::shared_ptr<Timer>;
    using Callback =std::function<bool()>;

    /**
    * 构造定时器
    * @param second 定时器重复秒数
    * @param cb 定时器任务，返回true表示重复下次任务，否则不重复，如果任务中抛异常，则默认重复下次任务
    * @param poller EventPoller对象，可以为nullptr
    */
    Timer(float second, const Callback &cb, const EventLoop::Ptr &poller);
    ~Timer();

  private:
    std::weak_ptr<EventLoop::DelayTask> _tag;
    //定时器保持EventPoller的强引用
    EventLoop::Ptr _poller;

//  public:
//    Timer(TimerCallback cb, Timestamp when, double interval)
//      : callback_(std::move(cb)),
//        expiration_(when),
//        interval_(interval),
//        repeat_(interval > 0.0),
//        sequence_(s_numCreated_.incrementAndGet()) {}
//
//    void run() const {
//      callback_();
//    }
//
//    Timestamp expiration() const { return expiration_; }
//
//    bool repeat() const { return repeat_; }
//
//    int64_t sequence() const { return sequence_; }
//
//    void restart(Timestamp now);
//
//    static int64_t numCreated() { return s_numCreated_.get(); }
//
//  private:
//    const TimerCallback callback_;
//    Timestamp expiration_;
//    const double interval_;
//    const bool repeat_;
//    const int64_t sequence_;
//
//    static AtomicInt64 s_numCreated_;
  };

RD_NAMESPACE_END

#endif //RENDU_TIMER_H
