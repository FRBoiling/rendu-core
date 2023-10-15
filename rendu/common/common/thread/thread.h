/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_THREAD_H
#define RENDU_THREAD_H

#include "base/non_copyable.h"
#include "base/atomic.h"
#include "count_down_latch.h"
#include "current_thread.h"

RD_NAMESPACE_BEGIN

  class Thread : NonCopyable {
  public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc, const string &name = string());

    // FIXME: make it movable in C++11
    ~Thread();

    void start();

    int join(); // return pthread_join()

    bool started() const { return started_; }

    // pthread_t pthreadId() const { return pthreadId_; }
    pid_t tid() const { return tid_; }

    const string &name() const { return name_; }

    static int numCreated() { return numCreated_.get(); }

  private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    string name_;
    CountDownLatch latch_;

    static AtomicInt32 numCreated_;
  };

RD_NAMESPACE_END

#endif //RENDU_THREAD_H
