/*
* Created by boil on 2024/1/29.
*/

#ifndef RENDU_THREAD_THREAD_COUNT_DOWN_LATCH_H_
#define RENDU_THREAD_THREAD_COUNT_DOWN_LATCH_H_

#include "thread_define.h"
#include "condition.h"
#include "mutex_lock.hpp"

THREAD_NAMESPACE_BEGIN

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

THREAD_NAMESPACE_END

#endif //RENDU_THREAD_THREAD_COUNT_DOWN_LATCH_H_
