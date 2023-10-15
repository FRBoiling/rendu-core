/*
* Created by boil on 2023/11/2.
*/

#include "posix_thread.h"


RD_NAMESPACE_BEGIN

  thread_local string thread_name;

  string LimitString(const char *name, size_t max_size) {
    string str = name;
    if (str.size() + 1 > max_size) {
      auto erased = str.size() + 1 - max_size + 3;
      str.replace(5, erased, "...");
    }
    return str;
  }

  string GetThreadName() {
    string ret;
    ret.resize(32);
    auto tid = pthread_self();
    pthread_getname_np(tid, (char *) ret.data(), ret.size());
    if (ret[0]) {
      ret.resize(strlen(ret.data()));
      return ret;
    }
    return std::to_string((uint64_t) tid);
  }

#if defined(__MACH__) || defined(__APPLE__)

  int32 gettid(void) {
//    mach_port_t
    return (int32) mach_thread_self();
  }

  void mac_setThreadName(const char *name) {
    pthread_setname_np(LimitString(name, 32).data());
  }

#else
  void linux_setThreadName(const char *name) {
    pthread_setname_np(pthread_self(), limitString(name, 16).data());
  }
#endif

RD_NAMESPACE_END