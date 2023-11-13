/*
* Created by boil on 2023/11/13.
*/

#include "time_helper.h"
#include "thread/thread_helper.h"

RD_NAMESPACE_BEGIN

uint64_t GetCurrentMicrosecondOrigin() {
#if !defined(_WIN32)
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec * 1000000LL + tv.tv_usec;
#else
  return  std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#endif
}


STRING GetTimeStr(const char *fmt, time_t time) {
  if (!time) {
    time = ::time(nullptr);
  }
  auto tm = GetLocalTime(time);
  size_t size = strlen(fmt) + 64;
  STRING ret;
  ret.resize(size);
  size = std::strftime(&ret[0], size, fmt, &tm);
  if (size > 0) {
    ret.resize(size);
  } else {
    ret = fmt;
  }
  return ret;
}

STime GetLocalTime(time_t sec) {
  STime tm;
#ifdef _WIN32
  localtime_s(&tm, &sec);
#else
  NoLocksLocalTime(&tm, sec);
#endif //_WIN32
  return tm;
}

RD_NAMESPACE_END