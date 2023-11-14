/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_COMMON_EVENT_LOOP_THREAD_H
#define RENDU_COMMON_EVENT_LOOP_THREAD_H

#include "thread/thread.h"

COMMON_NAMESPACE_BEGIN

  class EventLoop;

  class EventLoopThread : NonCopyable {
  public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const STRING &name = STRING());

    ~EventLoopThread();

    EventLoop *startLoop();

  private:
    void threadFunc();

    EventLoop *loop_
    GUARDED_BY(mutex_);
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_
    GUARDED_BY(mutex_);
    ThreadInitCallback callback_;
  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_EVENT_LOOP_THREAD_H
