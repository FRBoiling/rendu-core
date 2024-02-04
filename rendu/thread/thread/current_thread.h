/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_THREAD_CURRENT_THREAD_H
#define RENDU_THREAD_CURRENT_THREAD_H

#include "thread_define.h"

THREAD_NAMESPACE_BEGIN

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

void sleepUsec(int64_t usec);  // for testing

STRING stackTrace(bool demangle);

}  // namespace CurrentThread

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_CURRENT_THREAD_H
