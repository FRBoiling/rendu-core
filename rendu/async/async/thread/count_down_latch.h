/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_ASYNC_COUNT_DOWN_LATCH_H
#define RENDU_ASYNC_COUNT_DOWN_LATCH_H

#include "base/utils/non_copyable.h"
#include "condition.h"

ASYNC_NAMESPACE_BEGIN

  class CountDownLatch : NonCopyable {
  public:

    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

  private:
    mutable MutexLock mutex_;
    Condition condition_ GUARDED_BY(mutex_);
    int count_ GUARDED_BY(mutex_);
  };

ASYNC_NAMESPACE_END

#endif //RENDU_ASYNC_COUNT_DOWN_LATCH_H
