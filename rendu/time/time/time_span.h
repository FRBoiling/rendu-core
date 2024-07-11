#ifndef RENDU_TIME_TIME_TIME_SPAN_H_
#define RENDU_TIME_TIME_TIME_SPAN_H_

#include "time_point.h"

RD_TIME_NAMESPACE_BEGIN

class TimeSpan
    : public EqualityComparable<TimeSpan>,
      public LessThanComparable<TimeSpan> {
public:
  using Duration = detail::Microseconds;

public:
  TimeSpan(Long microseconds = 0);

  TimeSpan(Int days, Int hours, Int minutes = 0, Int seconds = 0, Long milliseconds = 0);

public:
  Int totalDays() const;
  Int totalHours() const;
  Int totalMinutes() const;
  Int totalSeconds() const;
  Long totalMilliseconds() const;
  Double totalMicroseconds() const;

  Int Days() const;
  Int Hours() const;
  Int Minutes() const;
  Int Seconds() const;
  Long Milliseconds() const;
  Double Microseconds() const;

  TimeSpan operator*(const Double &rhs) const;
  TimeSpan operator/(const Double &rhs) const;
  TimeSpan operator%(const TimeSpan &rhs) const;
  TimeSpan operator+(const TimeSpan &rhs) const;
  TimeSpan operator-(const TimeSpan &rhs) const;

  bool operator==(const TimeSpan &rhs) const {
    return m_duration == rhs.m_duration;
  }

  bool operator<(const TimeSpan &rhs) const {
    return m_duration < rhs.m_duration;
  }


public:
  static TimeSpan fromDays(Int days);
  static TimeSpan fromHours(Int hours);
  static TimeSpan fromMinutes(Int minutes);
  static TimeSpan fromSeconds(Int seconds);
  static TimeSpan fromMilliseconds(Long milliseconds);
  static TimeSpan fromMicroseconds(Double microseconds);

private:
  Duration m_duration;
};

RD_TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_TIME_SPAN_H_