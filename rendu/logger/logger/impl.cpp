/*
* Created by boil on 2023/11/2.
*/

#include "impl.h"
#include "common/utils/str_error.h"
#include "common/thread/current_thread.h"

RD_NAMESPACE_BEGIN

  __thread char t_time[64];
  __thread time_t t_lastSecond;
  extern TimeZone g_logTimeZone;

  Impl::Impl(LogLevel level, int savedErrno, const SourceFile &file, int line)
    : time_(Timestamp::now()),
      stream_(),
      level_(level),
      line_(line),
      basename_(file) {
    formatTime();
    CurrentThread::tid();
    stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    stream_ << T(GetLogLevelName(level), 6);
    if (savedErrno != 0) {
      stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
  }

  void Impl::finish() {
    stream_ << " - " << basename_ << ':' << line_ << '\n';
  }

  void Impl::formatTime() {
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond) {
      t_lastSecond = seconds;
      struct DateTime dt;
      if (g_logTimeZone.valid()) {
        dt = g_logTimeZone.toLocalTime(seconds);
      } else {
        dt = TimeZone::toUtcTime(seconds);
      }

      int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                         dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
      assert(len == 17);
      (void) len;
    }

    if (g_logTimeZone.valid()) {
      Fmt us(".%06d ", microseconds);
      assert(us.length() == 8);
      stream_ << T(t_time, 17) << T(us.data(), 8);
    } else {
      Fmt us(".%06dZ ", microseconds);
      assert(us.length() == 9);
      stream_ << T(t_time, 17) << T(us.data(), 9);
    }
  }

RD_NAMESPACE_END