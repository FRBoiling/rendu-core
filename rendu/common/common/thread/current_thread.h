/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_CURRENT_THREAD_H
#define RENDU_COMMON_CURRENT_THREAD_H

#include "posix_thread.h"
#include "win_thread.h"

COMMON_NAMESPACE_BEGIN

  namespace CurrentThread {
    // internal
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;

    void cacheTid();

    inline int tid() {
      if (__builtin_expect(t_cachedTid == 0, 0)) {
        cacheTid();
      }
      return t_cachedTid;
    }

    inline const char *tidString() // for logging
    {
      return t_tidString;
    }

    inline int tidStringLength() // for logging
    {
      return t_tidStringLength;
    }

    inline const char *name() {
      return t_threadName;
    }

    bool isMainThread();

    void sleepUsec(std::int64_t usec);  // for testing

    std::string stackTrace(bool demangle);
  }  // namespace CurrentThread

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_CURRENT_THREAD_H
