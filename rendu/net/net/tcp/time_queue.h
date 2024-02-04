///*
//* Created by boil on 2023/11/2.
//*/
//
//#ifndef RENDU_NET_TIME_QUEUE_H
//#define RENDU_NET_TIME_QUEUE_H
//
//#include "net_define.h"
//#include "callbacks.h"
//#include "channel.h"
//#include <set>
//
//NET_NAMESPACE_BEGIN
//
//  class EventLoop;
//
/////
///// A best efforts timer queue.
///// No guarantee that the callback will be on time.
/////
//  class TimerQueue : NonCopyable {
//  public:
//    explicit TimerQueue(EventLoop *loop);
//
//    ~TimerQueue();
//
//    ///
//    /// Schedules the callback to be run at given time,
//    /// repeats if @c interval > 0.0.
//    ///
//    /// Must be thread safe. Usually be called from other threads.
//    TimerId AddTimer(TimerCallback cb,
//                     Timestamp when,
//                     double interval);
//
//    void Cancel(TimerId timerId);
//
//  private:
//
//    // FIXME: use unique_ptr<Timer> instead of raw pointers.
//    // This requires heterogeneous comparison lookup (N3465) from C++14
//    // so that we can find an T* in a set<unique_ptr<T>>.
//    typedef std::pair<Timestamp, Timer *> Entry;
//    typedef std::set<Entry> TimerList;
//    typedef std::pair<Timer *, int64_t> ActiveTimer;
//    typedef std::set<ActiveTimer> ActiveTimerSet;
//
//    void addTimerInLoop(Timer *timer);
//
//    void cancelInLoop(TimerId timerId);
//
//    // called when timerfd alarms
//    void handleRead();
//
//    // move out all expired timers
//    std::vector<Entry> getExpired(Timestamp now);
//
//    void reset(const std::vector<Entry> &expired, Timestamp now);
//
//    bool insert(Timer *timer);
//
//    EventLoop *loop_;
//    const int timerfd_;
//    Channel timerfdChannel_;
//    // Timer list sorted by expiration
//    TimerList timers_;
//
//    // for cancel()
//    ActiveTimerSet activeTimers_;
//    bool callingExpiredTimers_; /* atomic */
//    ActiveTimerSet cancelingTimers_;
//  };
//
//NET_NAMESPACE_END
//
//#endif //RENDU_NET_TIME_QUEUE_H
