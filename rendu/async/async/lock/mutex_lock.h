/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_ASYNC_ASYNC_LOCK_MUTEX_LOCK_H_
#define RENDU_ASYNC_ASYNC_LOCK_MUTEX_LOCK_H_

#include "thread/thread_object.h"

RD_ASYNC_NAMESPACE_BEGIN

class MutexLock : public ThreadObject {
public:
  MutexLock() : locked_(false) {
    lock();
  }

  ~MutexLock() {
    if (isLocked()) {
      unlock();
    }
  }

  // 锁定互斥量。如果互斥量已经被锁定，再次调用此方法将导致未定义行为。
  void lock() {
    mutex_.lock();
    locked_ = true;
  }

  // 解锁互斥量。如果互斥量已经被解锁，再次调用此方法将导致未定义行为。
  void unlock() {
    mutex_.unlock();
    locked_ = false;
  }

  // 返回锁是否被获取。
  // 注意：此方法并不能保证在调用后互斥量的状态不变，
  // 即可能在此之后互斥量被其他线程获取或释放。
  bool isLocked() const {
    return locked_;
  }

  // 返回内部的互斥量对象。
  std::mutex &getMutex() {
    return mutex_;
  }

  // 禁用内部的锁跟踪，通常在锁的所有权被std::unique_lock
  // 或相关构造取得后调用。
  // 注意：调用此方法需要谨慎，以避免互斥量状态不一致的情况。
  void release() {
    locked_ = false;
  }

private:
  std::mutex mutex_;
  bool locked_;
};

RD_ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_LOCK_MUTEX_LOCK_H_
