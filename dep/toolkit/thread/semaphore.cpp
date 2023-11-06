/*
* Created by boil on 2023/10/30.
*/
#include "semaphore.h"

RD_NAMESPACE_BEGIN

  Semaphore::Semaphore(size_t initial) {
#if defined(HAVE_SEM)
    sem_init(&_sem, 0, initial);
#else
    _count = 0;
#endif
  }

  Semaphore::~Semaphore() {
#if defined(HAVE_SEM)
    sem_destroy(&_sem);
#endif
  }

  void Semaphore::Post(size_t n) {
#if defined(HAVE_SEM)
    while (n--) {
          sem_post(&_sem);
      }
#else
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    _count += n;
    if (n == 1) {
      _condition.notify_one();
    } else {
      _condition.notify_all();
    }
#endif
  }

  void Semaphore::Wait() {
#if defined(HAVE_SEM)
    sem_wait(&_sem);
#else
    std::unique_lock<std::recursive_mutex> lock(_mutex);
    while (_count == 0) {
      _condition.wait(lock);
    }
    --_count;
#endif
  }

RD_NAMESPACE_END



