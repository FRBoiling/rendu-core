///*
//* Created by boil on 2023/11/14.
//*/
//
//#ifndef RENDU_BASE_THREAD_MUTEX_H_
//#define RENDU_BASE_THREAD_MUTEX_H_
//
//#include "thread_define.h"
//#include <mutex>
//
//RD_NAMESPACE_BEGIN
//
//template<class Mtx = std::recursive_mutex>
//class MutexLock {
//public:
//  explicit MutexLock(bool enable = false) {
//    _enable = enable;
//  }
//
//  ~MutexLock() = default;
//
//  inline void lock() {
//    if (_enable) {
//      _mtx.lock();
//    }
//  }
//
//  inline void unlock() {
//    if (_enable) {
//      _mtx.unlock();
//    }
//  }
//
//private:
//  bool _enable;
//  Mtx _mtx;
//};
//
//RD_NAMESPACE_END
//
//
//#endif//RENDU_BASE_THREAD_MUTEX_H_
