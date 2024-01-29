/*
* Created by boil on 2024/1/29.
*/

#ifndef RENDU_ASYNC_THREAD_COUNT_DOWN_LATCH_H_
#define RENDU_ASYNC_THREAD_COUNT_DOWN_LATCH_H_

#include "async_define.h"
#include "condition.h"
#include "mutex_lock.hpp"

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

#endif //RENDU_ASYNC_THREAD_COUNT_DOWN_LATCH_H_
