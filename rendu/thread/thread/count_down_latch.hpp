/*
* Created by boil on 2024/1/29.
*/

#ifndef RENDU_THREAD_THREAD_COUNT_DOWN_LATCH_HPP_
#define RENDU_THREAD_THREAD_COUNT_DOWN_LATCH_HPP_

#include "thread_define.h"

#include "condition.hpp"
#include "mutex_lock.hpp"

THREAD_NAMESPACE_BEGIN

class CountDownLatch{
public:
  explicit CountDownLatch(int count) : count_(count){
  }

  void wait() {
    MutexLock lock;
    while (count_ > 0) {
      condition_.Wait(lock);
    }
  }

  void countDown() {
    MutexLock lock;
    if (count_ > 0) {
      --count_;
      if (count_ == 0) {
        condition_.NotifyAll();
      }
    }
  }

  int getCount() const {
    MutexLock lock;
    return count_;
  }

private:
  mutable MutexLock mutex_;
  Condition condition_;
  int count_;
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_COUNT_DOWN_LATCH_HPP_
