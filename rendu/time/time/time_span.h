#ifndef RENDU_TIME_TIME_TIME_SPAN_H_
#define RENDU_TIME_TIME_TIME_SPAN_H_

#include "date/date.h"
#include "time_define.h"

TIME_NAMESPACE_BEGIN

class TimeSpan {
public:
  using Duration = date::sys_time<std::chrono::milliseconds>;
  using Rep = std::chrono::milliseconds::rep;

  TimeSpan(INT64 ticks);

  TimeSpan(INT32 days = 0, INT32 hours = 0, INT32 minutes = 0, INT32 seconds = 0, INT64 milliseconds = 0);

public:
  INT32 TotalDays() const;
  INT32 TotalHours() const;
  INT32 TotalMinutes() const;
  INT32 TotalSeconds() const;
  INT64 TotalMilliseconds() const;
  Rep Ticks() const;

  INT32 Days() const;
  INT32 Hours() const;
  INT32 Minutes() const;
  INT32 Seconds() const;
  INT64 Milliseconds() const;

  TimeSpan operator*(const double &rhs) const;
  TimeSpan operator/(const double &rhs) const;
  TimeSpan operator%(const TimeSpan &rhs) const;

public:
  static TimeSpan FromDays(INT32 days);
  static TimeSpan FromHours(INT32 hours);
  static TimeSpan FromMinutes(INT32 minutes);
  static TimeSpan FromSeconds(INT32 seconds);
  static TimeSpan FromMilliseconds(INT64 milliseconds);

private:
  Duration m_duration;
};

TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_TIME_SPAN_H_