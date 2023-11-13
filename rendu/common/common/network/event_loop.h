/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_EVENT_LOOP_H
#define RENDU_COMMON_EVENT_LOOP_H

#include "base/utils/non_copyable.h"

#include "base/any/any.h"
#include "callbacks.h"
#include "pipe.h"
#include "thread/current_thread.h"
#include "thread/mutex_lock.h"
#include "time/time_id.h"
#include "time/timestamp.h"
#include "time_queue.h"

COMMON_NAMESPACE_BEGIN

  class Channel;

  class Poller;

  class TimerQueue;

  class EventLoop : NonCopyable {
  public:
    typedef std::function<void()> Functor;

    EventLoop();

    ~EventLoop();  // force out-line dtor, for std::unique_ptr members.

  public:
    ///
    /// Loops forever.
    ///
    /// Must be called in the same thread as creation of the object.
    ///
    void Loop();

    /// Quits loop.
    ///
    /// This is not 100% thread safe, if you call through a raw pointer,
    /// better to call through shared_ptr<EventLoop> for 100% safety.
    void Quit();

    ///
    /// Time when poll returns, usually means data arrival.
    ///
    Timestamp PollReturnTime() const { return pollReturnTime_; }

    int64_t Iteration() const { return iteration_; }

    /// Runs callback immediately in the loop thread.
    /// It wakes up the loop, and run the cb.
    /// If in the same loop thread, cb is run within the function.
    /// Safe to call from other threads.
    void RunInLoop(Functor cb);

    /// Queues callback in the loop thread.
    /// Runs after finish pooling.
    /// Safe to call from other threads.
    void QueueInLoop(Functor cb);

    size_t QueueSize() const;

    // timers

    ///
    /// Runs callback at 'time'.
    /// Safe to call from other threads.
    ///
    TimerId RunAt(Timestamp time, TimerCallback cb);

    ///
    /// Runs callback after @c delay seconds.
    /// Safe to call from other threads.
    ///
    TimerId RunAfter(double delay, TimerCallback cb);

    ///
    /// Runs callback every @c interval seconds.
    /// Safe to call from other threads.
    ///
    TimerId RunEvery(double interval, TimerCallback cb);

    ///
    /// Cancels the timer.
    /// Safe to call from other threads.
    ///
    void Cancel(TimerId timerId);

    // internal usage
    void Wakeup();

    void UpdateChannel(Channel *channel);

    void RemoveChannel(Channel *channel);

    bool HasChannel(Channel *channel);

    // pid_t threadId() const { return threadId_; }
    void assertInLoopThread() {
      if (!isInLoopThread()) {
        abortNotInLoopThread();
      }
    }

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    // bool callingPendingFunctors() const { return callingPendingFunctors_; }
    bool eventHandling() const { return eventHandling_; }

    void setContext(const Any &context) { context_ = context; }

    const Any &getContext() const { return context_; }

    Any *getMutableContext() { return &context_; }

    static EventLoop *getEventLoopOfCurrentThread();

  private:
    void abortNotInLoopThread();

    void handleRead();  // waked up
    void doPendingFunctors();

    void printActiveChannels() const; // DEBUG
  private:
    typedef std::vector<Channel *> ChannelList;

    bool looping_; /* atomic */
    std::atomic<bool> quit_;
    bool eventHandling_; /* atomic */
    bool callingPendingFunctors_; /* atomic */
    int64_t iteration_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    //内部事件管道
    Pipe wakeupPipe_;

    // unlike in TimerQueue, which is an internal class,
    // we don't expose Channel to client.
    std::unique_ptr<Channel> wakeupChannel_;
    Any context_;

    // scratch variables
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_
      GUARDED_BY(mutex_);
  };

RD_NAMESPACE_END

#endif //RENDU_COMMON_EVENT_LOOP_H
