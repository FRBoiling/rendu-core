/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_ASYNC_POSIX_THREAD_H
#define RENDU_ASYNC_POSIX_THREAD_H

#ifndef _WIN32

#include "async_define.h"
#include <pthread.h>

ASYNC_NAMESPACE_BEGIN


  std::string LimitString(const char *name, size_t max_size);

  std::string GetThreadName();


#ifdef __APPLE__

#include <mach/mach.h>

  std::int32_t gettid(void);

  void mac_setThreadName(const char *name);

#define  SetThreadName(name) mac_setThreadName(name)

#elif __linux__

#include <sys/prctl.h>

  void linux_setThreadName(const char *name);
  bool linux_setThreadAffinity(int i);

#define  SetThreadName(name) linux_setThreadName(name)

#endif

ASYNC_NAMESPACE_END

#endif

#endif //RENDU_ASYNC_POSIX_THREAD_H
