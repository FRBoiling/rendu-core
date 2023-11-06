/*
* Created by boil on 2023/11/2.
*/

#include "timestamp.h"

#include <sys/time.h>
#include <cinttypes>

COMMON_NAMESPACE_BEGIN

  static_assert(sizeof(Timestamp) == sizeof(INT64),
                "Timestamp should be same size as INT64");

  STRING Timestamp::toString() const {
    char buf[32] = {0};
    INT64 seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    INT64 microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf), "%"
    PRId64
    ".%06"
    PRId64
    "", seconds, microseconds);
    return buf;
  }

  STRING Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (showMicroseconds) {
      int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
      snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
               microseconds);
    } else {
      snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
  }

  Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    INT64 seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
  }

COMMON_NAMESPACE_END