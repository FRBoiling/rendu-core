/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_COMMON_EVENT_LOOP_THREAD_H
#define RENDU_COMMON_EVENT_LOOP_THREAD_H

#include "base/utils/non_copyable.h"
#include "thread/thread.h"

RD_NAMESPACE_BEGIN

  class EventLoop;

  class EventLoopThread : NonCopyable {
  public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const string &name = string());

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

RD_NAMESPACE_END

#endif //RENDU_COMMON_EVENT_LOOP_THREAD_H
