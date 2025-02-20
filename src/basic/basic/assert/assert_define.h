/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_BASIC_ASSERT_ASSERT_DEFINE_H
#define RENDU_BASIC_ASSERT_ASSERT_DEFINE_H

#include <iostream>
#include <chrono>
#include <iomanip>

#include "type/type_include.h"

RD_NAMESPACE_BEGIN

#ifdef _ENABLE_LIKELY_
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely
#define unlikely
#endif


template<typename T>
Status __ASSERT_NOT_NULL(T t) {
  return (unlikely(nullptr == t))
             ? ErrStatus(RD_INPUT_IS_NULL)
             : Status();
}

template<typename T, typename... Args>
Status __ASSERT_NOT_NULL(T t, Args... args) {
  if (unlikely(t == nullptr)) {
    return __ASSERT_NOT_NULL(t);
  }

  return __ASSERT_NOT_NULL(args...);
}

template<typename T>
Void __ASSERT_NOT_NULL_THROW_EXCEPTION(T t) {
  if (unlikely(nullptr == t)) {
    RD_THROW_EXCEPTION("[Exception] " + RD_INPUT_IS_NULL)
  }
}

template<typename T, typename... Args>
Void __ASSERT_NOT_NULL_THROW_EXCEPTION(T t, Args... args) {
  if (unlikely(nullptr == t)) {
    __ASSERT_NOT_NULL_THROW_EXCEPTION(t);
  }

  __ASSERT_NOT_NULL_THROW_EXCEPTION(args...);
}


/** 判断传入的多个指针信息，是否为空 */
#define RD_ASSERT_NOT_NULL(ptr, ...)                                                     \
    {                                                                                        \
        const Status& __cur_status__ = __ASSERT_NOT_NULL(ptr, ##__VA_ARGS__);               \
        if (unlikely(__cur_status__.isErr())) { return __cur_status__; }                     \
    }                                                                                        \


/** 判断传入的多个指针，是否为空。如果为空，则抛出异常信息 */
#define RD_ASSERT_NOT_NULL_THROW_ERROR(ptr, ...)                                         \
    __ASSERT_NOT_NULL_THROW_EXCEPTION(ptr, ##__VA_ARGS__);                                   \


/* 判断函数流程是否可以继续 */
static std::mutex g_check_status_mtx;
#define RD_FUNCTION_CHECK_STATUS                                                         \
    if (unlikely(status.isErr())) {                                                          \
        if (status.isCrash()) { throw Exception(status.getInfo()); }                        \
        RD_LOCK_GUARD lock{ g_check_status_mtx };                                        \
        RD_ECHO("%s, errorCode = [%d], errorInfo = [%s].",                               \
            status.getLocate().c_str(), status.getCode(), status.getInfo().c_str());         \
        return status;                                                                       \
    }                                                                                        \

/* 删除资源信息 */
#define RD_DELETE_PTR(ptr)                                                  \
    if (unlikely((ptr) != nullptr)) {                                           \
        delete (ptr);                                                           \
        (ptr) = nullptr;                                                        \
    }                                                                           \

#define RD_ASSERT_INIT(isInit)                                              \
    if (unlikely((isInit) != is_init_)) {                                       \
        RD_RETURN_ERROR_STATUS("init status is not suitable")               \
    }                                                                           \

#define RD_ASSERT_INIT_THROW_ERROR(isInit)                                  \
    if (unlikely((isInit) != is_init_)) {                                       \
        RD_THROW_EXCEPTION("[CException] init status is not suitable")      \
    }



/**
 * 定制化输出
 * @param cmd
 * @param ...
 * 注：内部包含全局锁，不建议正式上线的时候使用
 */
static std::mutex g_echo_mtx;
inline Void RD_ECHO(const char *cmd, ...) {
#ifdef _CGRAPH_SILENCE_
  return;
#endif

  std::lock_guard<std::mutex> lock{ g_echo_mtx };
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
  std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.")    \
            << std::setfill('0') << std::setw(3) << ms << "] ";

  va_list args;
  va_start(args, cmd);
  vprintf(cmd, args);
  va_end(args);
  std::cout << "\n";
}

RD_NAMESPACE_END

#endif//RENDU_BASIC_ASSERT_ASSERT_DEFINE_H
