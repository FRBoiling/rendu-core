/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_BASE_THREAD_HELPER_H
#define RENDU_BASE_THREAD_HELPER_H


#include "base_define.h"

#include <thread>
#include <unistd.h>

#include "mutex.h"
#include "semaphore.h"

#include "string/string_helper.h"

RD_NAMESPACE_BEGIN

  ALIAS(std::thread, THREAD);

  extern thread_local THREAD thread_name;

  void SetThreadName(const char *name);

  STRING GetThreadName();

  STRING LimitString(const char *name, size_t max_size);

  bool SetThreadAffinity(int i);

#if defined(_WIN32)
  void sleep(int second) {
    Sleep(1000 * second);
  }
  void usleep(int micro_seconds) {
    this_thread::sleep_for(std::chrono::microseconds(micro_seconds));
  }

  int gettimeofday(struct timeval *tp, void *tzp) {
    auto now_stamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    tp->tv_sec = (decltype(tp->tv_sec))(now_stamp / 1000000LL);
    tp->tv_usec = now_stamp % 1000000LL;
    return 0;
  }

  const char *strcasestr(const char *big, const char *little){
    STRING big_str = big;
    STRING little_str = little;
    strToLower(big_str);
    strToLower(little_str);
    auto pos = strstr(big_str.data(), little_str.data());
    if (!pos){
      return nullptr;
    }
    return big + (pos - big_str.data());
  }

  int vasprintf(char **strp, const char *fmt, va_list ap) {
    // _vscprintf tells you how big the buffer needs to be
    int len = _vscprintf(fmt, ap);
    if (len == -1) {
      return -1;
    }
    size_t size = (size_t)len + 1;
    char *str = (char*)malloc(size);
    if (!str) {
      return -1;
    }
    // _vsprintf_s is the "secure" version of vsprintf
    int r = vsprintf_s(str, len + 1, fmt, ap);
    if (r == -1) {
      free(str);
      return -1;
    }
    *strp = str;
    return r;
  }

  int asprintf(char **strp, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vasprintf(strp, fmt, ap);
    va_end(ap);
    return r;
  }

#endif //WIN32

#ifdef _WIN32
#define GetPid() GetCurrentProcessId()
#else
#define GetPid() getpid()
#endif

RD_NAMESPACE_END

#endif //RENDU_BASE_THREAD_HELPER_H
