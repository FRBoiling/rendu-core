/*
* Created by boil on 2023/11/2.
*/

#include "event_loop.h"

#include "poller.h"
#include "time_queue.h"
#include "logger/log.h"
#include "thread/mutex_lock.h"
#include "channel.h"
#include "sockets/sock_ops.h"

COMMON_NAMESPACE_BEGIN
  namespace {
    __thread EventLoop *t_loopInThisThread = 0;

    const int kPollTimeMs = 10000;

    int createEventfd() {
      //TODO:BOIL
//      int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
//      if (evtfd < 0) {
//        LOG_SYSERR << "Failed in eventfd";
//        abort();
//      }
//      return evtfd;
      return -1;
    }

#pragma GCC diagnostic ignored "-Wold-style-cast"

    class IgnoreSigPipe {
    public:
      IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
        // LOG_TRACE << "Ignore SIGPIPE";
      }
    };

#pragma GCC diagnostic error "-Wold-style-cast"

    IgnoreSigPipe initObj;
  }  // namespace



  EventLoop *EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
  }

  EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      eventHandling_(false),
      callingPendingFunctors_(false),
      iteration_(0),
      threadId_(CurrentThread::tid()),
      poller_(Poller::NewDefaultPoller(this)),
      timerQueue_(new TimerQueue(this)),
      wakeupPipe_(),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      currentActiveChannel_(NULL) {
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread) {
      LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                << " exists in this thread " << threadId_;
    } else {
      t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(
      std::bind(&EventLoop::handleRead, this));
    // we are always reading the wakeupfd
    wakeupChannel_->enableReading();
  }

  EventLoop::~EventLoop() {
    LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
              << " destructs in thread " << CurrentThread::tid();
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    SockOps::Close(wakeupFd_);
    t_loopInThisThread = NULL;
  }

  void EventLoop::Loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;  // FIXME: what if someone calls quit() before loop() ?
    LOG_TRACE << "EventLoop " << this << " start looping";

    while (!quit_) {
      activeChannels_.clear();
      pollReturnTime_ = poller_->Poll(kPollTimeMs, &activeChannels_);
      ++iteration_;
      if (Logger::logLevel() <= LogLevel::TRACE) {
        printActiveChannels();
      }
      // TODO sort channel by priority
      eventHandling_ = true;
      for (Channel *channel: activeChannels_) {
        currentActiveChannel_ = channel;
        currentActiveChannel_->handleEvent(pollReturnTime_);
      }
      currentActiveChannel_ = NULL;
      eventHandling_ = false;
      doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
  }

  void EventLoop::Quit() {
    quit_ = true;
    // There is a chance that loop() just executes while(!quit_) and exits,
    // then EventLoop destructs, then we are accessing an invalid object.
    // Can be fixed using mutex_ in both places.
    if (!isInLoopThread()) {
      Wakeup();
    }
  }

  void EventLoop::RunInLoop(Functor cb) {
    if (isInLoopThread()) {
      cb();
    } else {
      QueueInLoop(std::move(cb));
    }
  }

  void EventLoop::QueueInLoop(Functor cb) {
    {
      MutexLockGuard lock(mutex_);
      pendingFunctors_.push_back(std::move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_) {
      Wakeup();
    }
  }

  size_t EventLoop::QueueSize() const {
    MutexLockGuard lock(mutex_);
    return pendingFunctors_.size();
  }

  TimerId EventLoop::RunAt(Timestamp time, TimerCallback cb) {
//    return timerQueue_->addTimer(std::move(cb), time, 0.0);
    //TODO:BOIL
    LOG_TRACE << "RunAt";
    return TimerId();
  }

  TimerId EventLoop::RunAfter(double delay, TimerCallback cb) {
//    Timestamp time(addTime(Timestamp::now(), delay));
//    return RunAt(time, std::move(cb));
    //TODO:BOIL
    LOG_TRACE << "RunAfter";
    return TimerId();
  }

  TimerId EventLoop::RunEvery(double interval, TimerCallback cb) {
//    Timestamp time(addTime(Timestamp::now(), interval));
//    return timerQueue_->addTimer(std::move(cb), time, interval);
    //TODO:BOIL
    LOG_TRACE << "RunEvery";
    return TimerId();
  }

  void EventLoop::Cancel(TimerId timerId) {
//    return timerQueue_->Cancel(timerId);
  }

  void EventLoop::UpdateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->UpdateChannel(channel);
  }

  void EventLoop::RemoveChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_) {
      assert(currentActiveChannel_ == channel ||
             std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->RemoveChannel(channel);
  }

  bool EventLoop::HasChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->HasChannel(channel);
  }

  void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
  }

  void EventLoop::Wakeup() {
//    uint64_t one = 1;
//    ssize_t n = SocketOps::Write(wakeupFd_, &one, sizeof one);
//    if (n != sizeof one) {
//      LOG_ERROR << "EventLoop::Wakeup() writes " << n << " bytes instead of 8";
//    }
  }

  void EventLoop::handleRead() {
//    uint64_t one = 1;
//    ssize_t n = SocketOps::Read(wakeupFd_, &one, sizeof one);
//    if (n != sizeof one) {
//      LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
//    }
  }

  void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
      MutexLockGuard lock(mutex_);
      functors.swap(pendingFunctors_);
    }

    for (const Functor &functor: functors) {
      functor();
    }
    callingPendingFunctors_ = false;
  }

  void EventLoop::printActiveChannels() const {
    for (const Channel *channel: activeChannels_) {
      LOG_TRACE << "{" << channel->reventsToString() << "} ";
    }
  }

COMMON_NAMESPACE_END
