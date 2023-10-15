/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_THREAD_DATA_H
#define RENDU_THREAD_DATA_H


#include "count_down_latch.h"
#include "atomic.h"
#include "current_thread.h"
#include "exception.h"
#include "thread.h"

RD_NAMESPACE_BEGIN

  struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(ThreadFunc func,
               const string &name,
               pid_t *tid,
               CountDownLatch *latch);

    void runInThread();
  };

RD_NAMESPACE_END

#endif //RENDU_THREAD_DATA_H
