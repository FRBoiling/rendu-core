/*
* Created by boil on 2024/1/29.
*/

#ifndef RENDU_THREAD_THREAD_CONDITION_H_
#define RENDU_THREAD_THREAD_CONDITION_H_

#include "thread_define.h"
#include "mutex_lock.hpp"

THREAD_NAMESPACE_BEGIN

class Condition : NonCopyable
{
 public:
  explicit Condition(MutexLock& mutex)
      : mutex_(mutex)
  {
    MCHECK(pthread_cond_init(&pcond_, NULL));
  }

  ~Condition()
  {
    MCHECK(pthread_cond_destroy(&pcond_));
  }

  void wait()
  {
    MutexLock::UnassignGuard ug(mutex_);
    MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
  }

  // returns true if time out, false otherwise.
  bool waitForSeconds(double seconds);

  void notify()
  {
    MCHECK(pthread_cond_signal(&pcond_));
  }

  void notifyAll()
  {
    MCHECK(pthread_cond_broadcast(&pcond_));
  }

 private:
  MutexLock& mutex_;
  pthread_cond_t pcond_;
};

THREAD_NAMESPACE_END

#endif //RENDU_THREAD_THREAD_CONDITION_H_
