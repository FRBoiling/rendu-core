/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_THREAD_DATA_H
#define RENDU_COMMON_THREAD_DATA_H


#include "base/atomic/atomic.h"
#include "count_down_latch.h"
#include "current_thread.h"
#include "thread.h"

COMMON_NAMESPACE_BEGIN

  struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(ThreadFunc func,
               const std::string &name,
               pid_t *tid,
               CountDownLatch *latch);

    void runInThread();
  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_THREAD_DATA_H
