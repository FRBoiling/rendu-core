/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_BASE_BASE_THREAD_MUTEX_H_
#define RENDU_BASE_BASE_THREAD_MUTEX_H_

#include "base_define.h"
#include <mutex>

RD_NAMESPACE_BEGIN

template<class Mtx = std::recursive_mutex>
class Mutex {
public:
  explicit Mutex(bool enable) {
    _enable = enable;
  }

  ~Mutex() = default;

  inline void lock() {
    if (_enable) {
      _mtx.lock();
    }
  }

  inline void unlock() {
    if (_enable) {
      _mtx.unlock();
    }
  }

private:
  bool _enable;
  Mtx _mtx;
};

RD_NAMESPACE_END


#endif//RENDU_BASE_BASE_THREAD_MUTEX_H_
