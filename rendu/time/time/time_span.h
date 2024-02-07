#ifndef RENDU_TIME_TIME_TIME_SPAN_H_
#define RENDU_TIME_TIME_TIME_SPAN_H_

#include "date/date.h"
#include "time_define.h"

TIME_NAMESPACE_BEGIN

class TimeSpan {
public:
  using Duration = date::sys_time<std::chrono::milliseconds>;
  using Rep = std::chrono::milliseconds::rep;

  TimeSpan(Long ticks);

  TimeSpan(int days = 0, int hours = 0, int minutes = 0, int seconds = 0, Long milliseconds = 0);

public:
  int TotalDays() const;
  int TotalHours() const;
  int TotalMinutes() const;
  int TotalSeconds() const;
  Long TotalMilliseconds() const;
  Rep Ticks() const;

  int Days() const;
  int Hours() const;
  int Minutes() const;
  int Seconds() const;
  Long Milliseconds() const;

  TimeSpan operator*(const double &rhs) const;
  TimeSpan operator/(const double &rhs) const;
  TimeSpan operator%(const TimeSpan &rhs) const;

public:
  static TimeSpan FromDays(int days);
  static TimeSpan FromHours(int hours);
  static TimeSpan FromMinutes(int minutes);
  static TimeSpan FromSeconds(int seconds);
  static TimeSpan FromMilliseconds(Long milliseconds);

private:
  Duration m_duration;
};

TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_TIME_SPAN_H_