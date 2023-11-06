/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_THREAD_HELPER_H
#define RENDU_THREAD_HELPER_H

#include <thread>
#include <mutex>
#include "define.h"
#include <string>
#include <unistd.h>

RD_NAMESPACE_BEGIN

  class ThreadHelper {
  public:
    static thread_local string thread_name;

    static void SetThreadName(const char *name);

    static string GetThreadName();

    static string LimitString(const char *name, size_t max_size);

    static bool SetThreadAffinity(int i);

  };

#ifdef _WIN32
#define GetPid() GetCurrentProcessId()
#else
#define GetPid() getpid()
#endif

RD_NAMESPACE_END

#endif //RENDU_THREAD_HELPER_H
