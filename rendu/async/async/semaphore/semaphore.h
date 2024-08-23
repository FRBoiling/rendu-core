/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_ASYNC_ASYNC_SEMAPHORE_SEMAPHORE_H_
#define RENDU_ASYNC_ASYNC_SEMAPHORE_SEMAPHORE_H_

#include "lock/lock_include.h"

RD_NAMESPACE_BEGIN

class Semaphore : public ThreadObject {
public:
  explicit Semaphore(int initial_count = 0)
      : count_(initial_count) {}

  /**
     * 触发一次信号
     */
  Void signal() {
    RD_UNIQUE_LOCK lk(mutex_);
    count_++;
    if (count_ <= 0) {
      cv_.notify_one();
    }
  }

  /**
     * 等待信号触发
     */
  Void wait() {
    RD_UNIQUE_LOCK lk(mutex_);
    cv_.wait(lk, [this]() { return count_ > 0; });
    --count_;
  }

private:
  int count_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_SEMAPHORE_SEMAPHORE_H_
