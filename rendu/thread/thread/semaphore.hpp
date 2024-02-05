/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_THREAD_SEMAPHORE_H_
#define RENDU_THREAD_SEMAPHORE_H_

#include "thread_define.h"
#include "mutex_lock.hpp"
#include "condition.hpp"

THREAD_NAMESPACE_BEGIN

class Semaphore {
public:
  explicit Semaphore(int initial_count = 0)
      : count_(initial_count) {}

  void Acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this](){ return count_ > 0; });
    --count_;
  }

  void Release() {
    std::unique_lock<std::mutex> lock(mutex_);
    ++count_;
    condition_.notify_one();
  }

private:
  int count_;
  std::mutex mutex_;
  std::condition_variable condition_;
};

THREAD_NAMESPACE_END

#endif /* RENDU_THREAD_SEMAPHORE_H_ */
