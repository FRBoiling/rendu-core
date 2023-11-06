/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_POSIX_THREAD_H
#define RENDU_COMMON_POSIX_THREAD_H

#ifndef _WIN32

#include "common/common_define.h"
#include <pthread.h>

COMMON_NAMESPACE_BEGIN


  STRING LimitString(const char *name, size_t max_size);

  STRING GetThreadName();


#ifdef __APPLE__

#include <mach/mach.h>

  INT32 gettid(void);

  void mac_setThreadName(const char *name);

#define  SetThreadName(name) mac_setThreadName(name)

#elif __linux__

#include <sys/prctl.h>

  void linux_setThreadName(const char *name);
  bool linux_setThreadAffinity(int i);

#define  SetThreadName(name) linux_setThreadName(name)

#endif

COMMON_NAMESPACE_END

#endif

#endif //RENDU_COMMON_POSIX_THREAD_H
