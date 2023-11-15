/*
* Created by boil on 2023/11/2.
*/

#include "posix_thread.h"

ASYNC_NAMESPACE_BEGIN

    thread_local std::string thread_name;

    std::string LimitString(const char *name, size_t max_size) {
      std::string str = name;
      if (str.size() + 1 > max_size) {
        auto erased = str.size() + 1 - max_size + 3;
        str.replace(5, erased, "...");
      }
      return str;
    }

    std::string GetThreadName() {
      std::string ret;
      ret.resize(32);
      auto tid = pthread_self();
      pthread_getname_np(tid, (char *) ret.data(), ret.size());
      if (ret[0]) {
        ret.resize(strlen(ret.data()));
        return ret;
      }
      return std::to_string((std::uint64_t)tid);
    }

#if defined(__MACH__) || defined(__APPLE__)

    std::int32_t gettid(void) {
//    mach_port_t
      return (std::int32_t) mach_thread_self();
    }

    void mac_setThreadName(const char *name) {
      pthread_setname_np(LimitString(name, 32).data());
    }

#else
    void linux_setThreadName(const char *name) {
      pthread_setname_np(pthread_self(), limitString(name, 16).data());
    }
#endif

ASYNC_NAMESPACE_END