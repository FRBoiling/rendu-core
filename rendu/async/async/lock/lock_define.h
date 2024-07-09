/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_LOCK_LOCK_DEFINE_H_
#define RENDU_ASYNC_ASYNC_LOCK_LOCK_DEFINE_H_

#if __cplusplus >= 201703L
#include <shared_mutex>
#else
#include <mutex>
#endif
using RD_LOCK_GUARD = std::lock_guard<std::mutex>;
using RD_UNIQUE_LOCK = std::unique_lock<std::mutex>;

#if __cplusplus >= 201703L
using RD_READ_LOCK = std::shared_lock<std::shared_mutex>;
using RD_WRITE_LOCK = std::unique_lock<std::shared_mutex>;
#else
using RD_READ_LOCK = RD_LOCK_GUARD;    // C++14不支持读写锁，使用mutex替代
using RD_WRITE_LOCK = RD_LOCK_GUARD;
#endif


#endif//RENDU_ASYNC_ASYNC_LOCK_LOCK_DEFINE_H_
