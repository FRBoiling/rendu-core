/*
* Created by boil on 2023/10/26.
*/

#include "timer_helper.h"
#include "thread/thread_helper.h"
#include "logger/logger.h"

RD_NAMESPACE_BEGIN

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
    string big_str = big;
    string little_str = little;
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

  static long s_gmtoff = 0; //时间差
  static OnceToken s_token([]() {
#ifdef _WIN32
    TIME_ZONE_INFORMATION tzinfo;
    DWORD dwStandardDaylight;
    long bias;
    dwStandardDaylight = GetTimeZoneInformation(&tzinfo);
    bias = tzinfo.Bias;
    if (dwStandardDaylight == TIME_ZONE_ID_STANDARD) {
        bias += tzinfo.StandardBias;
    }
    if (dwStandardDaylight == TIME_ZONE_ID_DAYLIGHT) {
        bias += tzinfo.DaylightBias;
    }
    s_gmtoff = -bias * 60; //时间差(分钟)
#else
    LocalTimeInit();
    s_gmtoff = GetLocalTime(time(nullptr)).tm_gmtoff;
#endif // _WIN32
  });

  long GetGMTOff() {
    return s_gmtoff;
  }

  static inline uint64_t getCurrentMicrosecondOrigin() {
#if !defined(_WIN32)
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
#else
    return  std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#endif
  }

  static std::atomic<uint64_t> s_currentMicrosecond(0);
  static std::atomic<uint64_t> s_currentMillisecond(0);
  static std::atomic<uint64_t> s_currentMicrosecond_system(getCurrentMicrosecondOrigin());
  static std::atomic<uint64_t> s_currentMillisecond_system(getCurrentMicrosecondOrigin() / 1000);

  static inline bool initMillisecondThread() {
    static std::thread s_thread([]() {
      ThreadHelper::SetThreadName("stamp thread");
      DebugL << "Stamp thread started";
      uint64_t last = getCurrentMicrosecondOrigin();
      uint64_t now;
      uint64_t microsecond = 0;
      while (true) {
        now = getCurrentMicrosecondOrigin();
        //记录系统时间戳，可回退
        s_currentMicrosecond_system.store(now, std::memory_order_release);
        s_currentMillisecond_system.store(now / 1000, std::memory_order_release);

        //记录流逝时间戳，不可回退
        int64_t expired = now - last;
        last = now;
        if (expired > 0 && expired < 1000 * 1000) {
          //流逝时间处于0~1000ms之间，那么是合理的，说明没有调整系统时间
          microsecond += expired;
          s_currentMicrosecond.store(microsecond, std::memory_order_release);
          s_currentMillisecond.store(microsecond / 1000, std::memory_order_release);
        } else if (expired != 0) {
          WarnL << "Stamp expired is abnormal: " << expired;
        }
        //休眠0.5 ms
        usleep(500);
      }
    });
    static OnceToken s_token([]() {
      s_thread.detach();
    });
    return true;
  }

  uint64_t GetCurrentMillisecond(bool system_time) {
    static bool flag = initMillisecondThread();
    if (system_time) {
      return s_currentMillisecond_system.load(std::memory_order_acquire);
    }
    return s_currentMillisecond.load(std::memory_order_acquire);
  }

  uint64_t GetCurrentMicrosecond(bool system_time) {
    static bool flag = initMillisecondThread();
    if (system_time) {
      return s_currentMicrosecond_system.load(std::memory_order_acquire);
    }
    return s_currentMicrosecond.load(std::memory_order_acquire);
  }

  string GetTimeStr(const char *fmt, time_t time) {
    if (!time) {
      time = ::time(nullptr);
    }
    auto tm = GetLocalTime(time);
    size_t size = strlen(fmt) + 64;
    string ret;
    ret.resize(size);
    size = std::strftime(&ret[0], size, fmt, &tm);
    if (size > 0) {
      ret.resize(size);
    } else {
      ret = fmt;
    }
    return ret;
  }


  struct tm GetLocalTime(time_t sec) {
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &sec);
#else
    NoLocksLocalTime(&tm, sec);
#endif //_WIN32
    return tm;
  }

RD_NAMESPACE_END