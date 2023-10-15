/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_EVENT_LOOP_THREAD_POOL_H
#define RENDU_EVENT_LOOP_THREAD_POOL_H

#include "base/non_copyable.h"

RD_NAMESPACE_BEGIN

  class EventLoop;

  class EventLoopThread;

  class EventLoopThreadPool : NonCopyable {
  public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop *baseLoop, const string &nameArg);

    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }

    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // valid after calling start()
    /// round-robin
    EventLoop *getNextLoop();

    /// with the same hash code, it will always return the same EventLoop
    EventLoop *getLoopForHash(size_t hashCode);

    std::vector<EventLoop *> getAllLoops();

    bool started() const { return started_; }

    const string &name() const { return name_; }

  private:

    EventLoop *baseLoop_;
    string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
  };

RD_NAMESPACE_END

#endif //RENDU_EVENT_LOOP_THREAD_POOL_H
