/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_MUTEX_WRAPPER_H
#define RENDU_MUTEX_WRAPPER_H

#include "define.h"
#include <mutex>

RD_NAMESPACE_BEGIN

  template<class Mtx = std::recursive_mutex>
  class MutexWrapper {
  public:
    explicit MutexWrapper(bool enable) {
      _enable = enable;
    }

    ~MutexWrapper() = default;

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

#endif //RENDU_MUTEX_WRAPPER_H
