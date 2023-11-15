/*
* Created by boil on 2023/11/3.
*/

#include "event_loop_thread.h"
#include "event_loop.h"

NET_NAMESPACE_BEGIN

  EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                   const std::string &name)
    : loop_(NULL),
      exiting_(false),
      thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      mutex_(),
      cond_(mutex_),
      callback_(cb) {
  }

  EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
    {
      // still a tiny chance to call destructed object, if threadFunc exits just now.
      // but when EventLoopThread destructs, usually programming is exiting anyway.
      loop_->Quit();
      thread_.join();
    }
  }

  EventLoop *EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();

    EventLoop *loop = NULL;
    {
      MutexLockGuard lock(mutex_);
      while (loop_ == NULL) {
        cond_.wait();
      }
      loop = loop_;
    }

    return loop;
  }

  void EventLoopThread::threadFunc() {
    EventLoop loop;

    if (callback_) {
      callback_(&loop);
    }

    {
      MutexLockGuard lock(mutex_);
      loop_ = &loop;
      cond_.notify();
    }

    loop.Loop();
    //assert(exiting_);
    MutexLockGuard lock(mutex_);
    loop_ = NULL;
  }

NET_NAMESPACE_END