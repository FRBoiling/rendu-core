/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_ASYNC_ASYNC_LOCK_SPIN_LOCK_H_
#define RENDU_ASYNC_ASYNC_LOCK_SPIN_LOCK_H_

#include "thread/thread_object.h"
#include <atomic>

RD_ASYNC_NAMESPACE_BEGIN

class SpinLock : public ThreadObject {
public:
  /**
     * 加锁
     */
  Void lock() {
    // memory_order_acquire 后面访存指令勿重排至此条指令之前
    while (flag_.test_and_set(std::memory_order_acquire)) {
    }
  }

  /**
     * 解锁
     */
  Void unlock() {
    // memory_order_release 前面访存指令勿重排到此条指令之后
    flag_.clear(std::memory_order_release);
  }

  /**
     * 尝试加锁。若未加锁，会上锁
     * @return
     */
  Bool tryLock() {
    return !flag_.test_and_set();
  }

private:
  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;// 标志位
};

RD_ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_LOCK_SPIN_LOCK_H_
