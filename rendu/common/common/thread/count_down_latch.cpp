/*
* Created by boil on 2023/11/2.
*/

#include "count_down_latch.h"

COMMON_NAMESPACE_BEGIN

  CountDownLatch::CountDownLatch(int count)
    : mutex_(),
      condition_(mutex_),
      count_(count) {
  }

  void CountDownLatch::wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0) {
      condition_.wait();
    }
  }

  void CountDownLatch::countDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0) {
      condition_.notifyAll();
    }
  }

  int CountDownLatch::getCount() const {
    MutexLockGuard lock(mutex_);
    return count_;
  }

COMMON_NAMESPACE_END