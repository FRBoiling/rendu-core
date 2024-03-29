/*
* Created by boil on 2023/11/2.
*/

#include "time_queue.h"
#include "time/timer.h"
#include "sockets/sock_ops.h"
#include "event_loop.h"

NET_NAMESPACE_BEGIN

  namespace detail {

    int createTimerfd() {
      //TODO:BOIL
      return 0;
//      int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
//                                     TFD_NONBLOCK | TFD_CLOEXEC);
//      if (timerfd < 0) {
//        LOG_SYSFATAL << "Failed in timerfd_create";
//      }
//      return timerfd;
    }

    struct timespec howMuchTimeFromNow(Timestamp when) {
      int64_t microseconds = when.microSecondsSinceEpoch()
                             - Timestamp::now().microSecondsSinceEpoch();
      if (microseconds < 100) {
        microseconds = 100;
      }
      struct timespec ts;
      ts.tv_sec = static_cast<time_t>(
        microseconds / Timestamp::kMicroSecondsPerSecond);
      ts.tv_nsec = static_cast<long>(
        (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
      return ts;
    }

    void readTimerfd(int timerfd, Timestamp now) {
      uint64_t howmany;
      ssize_t n = SockOps::Read(timerfd, &howmany, sizeof howmany);
      LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
      if (n != sizeof howmany) {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
      }
    }

    void resetTimerfd(int timerfd, Timestamp expiration) {
      // wake up loop by timerfd_settime()
//      struct itimerspec newValue;
//      struct itimerspec oldValue;
//      memZero(&newValue, sizeof newValue);
//      memZero(&oldValue, sizeof oldValue);
//      newValue.it_value = howMuchTimeFromNow(expiration);
//      int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
      int ret = 0;
      if (ret) {
        LOG_CRITICAL << "timerfd_settime()";
      }
    }

  }  // namespace detail

  TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(detail::createTimerfd()),
      timerfdChannel_(loop, timerfd_),
      timers_(),
      callingExpiredTimers_(false) {
    timerfdChannel_.setReadCallback(
      std::bind(&TimerQueue::handleRead, this));
    // we are always reading the timerfd, we disarm it with timerfd_settime.
    timerfdChannel_.enableReading();
  }

  TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    SockOps::Close(timerfd_);
    // do not remove channel, since we're in EventLoop::dtor();
    for (const Entry &timer: timers_) {
      delete timer.second;
    }
  }

  TimerId TimerQueue::addTimer(TimerCallback cb,
                               Timestamp when,
                               double interval) {
    Timer *timer = new Timer(std::move(cb), when, interval);
    loop_->RunInLoop(
      std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
  }

  void TimerQueue::cancel(TimerId timerId) {
    loop_->RunInLoop(
      std::bind(&TimerQueue::cancelInLoop, this, timerId));
  }

  void TimerQueue::addTimerInLoop(Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged) {
      detail::resetTimerfd(timerfd_, timer->expiration());
    }
  }

  void TimerQueue::cancelInLoop(TimerId timerId) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
      size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
      assert(n == 1);
      (void) n;
      delete it->first; // FIXME: no delete please
      activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
      cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
  }

  void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    detail::readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    // safe to callback outside critical section
    for (const Entry &it: expired) {
      it.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
  }

  std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    assert(timers_.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(sentry);
    assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for (const Entry &it: expired) {
      ActiveTimer timer(it.second, it.second->sequence());
      size_t n = activeTimers_.erase(timer);
      assert(n == 1);
      (void) n;
    }

    assert(timers_.size() == activeTimers_.size());
    return expired;
  }

  void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now) {
    Timestamp nextExpire;

    for (const Entry &it: expired) {
      ActiveTimer timer(it.second, it.second->sequence());
      if (it.second->repeat()
          && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
        it.second->restart(now);
        insert(it.second);
      } else {
        // FIXME move to a free list
        delete it.second; // FIXME: no delete please
      }
    }

    if (!timers_.empty()) {
      nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
      detail::resetTimerfd(timerfd_, nextExpire);
    }
  }

  bool TimerQueue::insert(Timer *timer) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
      earliestChanged = true;
    }
    {
      std::pair<TimerList::iterator, bool> result
        = timers_.insert(Entry(when, timer));
      assert(result.second);
      (void) result;
    }
    {
      std::pair<ActiveTimerSet::iterator, bool> result
        = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
      assert(result.second);
      (void) result;
    }

    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
  }

NET_NAMESPACE_END