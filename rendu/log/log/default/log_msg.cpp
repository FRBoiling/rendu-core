/*
* Created by boil on 2023/12/23.
*/

#include "log_msg.h"
#include <iostream>
#include <sstream>
#include <thread>

LOG_NAMESPACE_BEGIN
namespace detail {

  uLong GetCurrThreadId() {
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__)
#if defined(__ANDROID__) && defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
#define SYS_gettid __NR_gettid
#endif
    return static_cast<size_t>(::syscall(SYS_gettid));
#elif defined(_AIX)
    struct __pthrdsinfo buf;
    int reg_size = 0;
    pthread_t pt = pthread_self();
    int retval = pthread_getthrds_np(&pt, PTHRDSINFO_QUERY_TID, &buf, sizeof(buf), NULL, &reg_size);
    int tid = (!retval) ? buf.__pi_tid : 0;
    return static_cast<size_t>(tid);
#elif defined(__DragonFly__) || defined(__FreeBSD__)
    return static_cast<size_t>(::pthread_getthreadid_np());
#elif defined(__NetBSD__)
    return static_cast<size_t>(::_lwp_self());
#elif defined(__OpenBSD__)
    return static_cast<size_t>(::getthrid());
#elif defined(__sun)
    return static_cast<size_t>(::thr_self());
#elif __APPLE__
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return tid;
#else// Default to standard C++11 (other Unix)
    return static_cast<size_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
  }
}// namespace detail

string ThreadIdToString(std::thread::id id) {
  std::ostringstream oss;
  oss << id;
  return oss.str();
}

LogMsg::LogMsg(LogMsgTime log_time, LogMsgSource source, string_view_t logger_name, LogLevel lvl, string_view_t content)
    : time_(log_time), source_(source), logger_name_(logger_name), lvl_(lvl), content_(content), thread_id_(detail::GetCurrThreadId()) {
}


std::string LogMsg::ToString(std::string fmt) {
  auto rrr = fmt::format(DEFAULT_PATTERN,
                         time_.ToString(),
                         logger_name_,
                         GetLogLevelSimpleName(lvl_),
                         thread_id_,
                         source_.GetFunctionName(),
                         source_.GetFileName(),
                         source_.GetLine(),
                         content_);

  return rrr;
}


LOG_NAMESPACE_END