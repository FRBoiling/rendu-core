/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_THREAD_DATA_H
#define RENDU_COMMON_THREAD_DATA_H


#include "count_down_latch.h"
#include "common/utils/atomic.h"
#include "current_thread.h"
#include "thread.h"

COMMON_NAMESPACE_BEGIN

  struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    STRING name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(ThreadFunc func,
               const STRING &name,
               pid_t *tid,
               CountDownLatch *latch);

    void runInThread();
  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_THREAD_DATA_H
