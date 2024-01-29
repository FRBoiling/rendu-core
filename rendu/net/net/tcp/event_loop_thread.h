/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_NET_EVENT_LOOP_THREAD_H
#define RENDU_NET_EVENT_LOOP_THREAD_H

#include "net_define.h"

NET_NAMESPACE_BEGIN

  class EventLoop;

  class EventLoopThread : NonCopyable {
  public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const std::string &name = std::string());

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

NET_NAMESPACE_END

#endif //RENDU_NET_EVENT_LOOP_THREAD_H
