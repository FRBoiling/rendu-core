/*
* Created by boil on 2024/1/29.
*/

#ifndef RENDU_THREAD_THREAD_CONDITION_HPP_
#define RENDU_THREAD_THREAD_CONDITION_HPP_

#include "thread_define.h"
#include "mutex_lock.hpp"
#include <condition_variable>

THREAD_NAMESPACE_BEGIN

class Condition {
public:
  // 阻止当前线程并等待条件被触发。
  // 在等待过程中，所提供的锁会自动地被释放，而在函数返回前又会被重新获取。
  // 注意：在调用此方法时，确保所提供的锁已经被锁定。
  void Wait(MutexLock &lock) {
    std::unique_lock<std::mutex> ulock(lock.GetMutex(), std::adopt_lock);
    cond_var.wait(ulock);
    lock.Release();
  }

  // 阻止当前线程并等待指定条件满足时返回。
  // 具体行为同上一方法，区别是这是一个模板方法，可以接收一个自定义的谓词，
  // 具体满足何种条件时返回由谓词决定。
  template<class Predicate>
  void Wait(MutexLock &lock, Predicate pred) {
    std::unique_lock<std::mutex> ulock(lock.GetMutex(), std::adopt_lock);
    cond_var.wait(ulock, pred);
    lock.Release();
  }

  // 唤醒一个正在等待的线程。
  void NotifyOne() {
    cond_var.notify_one();
  }

  // 唤醒所有正在等待的线程。
  void NotifyAll() {
    cond_var.notify_all();
  }

private:
  std::condition_variable cond_var;
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_CONDITION_HPP_
