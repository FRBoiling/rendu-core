/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_BASE_SEMAPHORE_H_
#define RENDU_BASE_SEMAPHORE_H_

#include "base_define.h"
#include <mutex>
#include <condition_variable>

RD_NAMESPACE_BEGIN

  class Semaphore {
  public:
    explicit Semaphore(size_t initial = 0);

    ~Semaphore();

    void Post(size_t n = 1);

    void Wait();

  private:
#if defined(HAVE_SEM)
    sem_t _sem;
#else
    size_t _count;
    std::recursive_mutex _mutex;
    std::condition_variable_any _condition;
#endif
  };

RD_NAMESPACE_END

#endif /* RENDU_BASE_SEMAPHORE_H_ */
