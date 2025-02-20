#ifndef RENDU_TIME_TIME_TIME_SPAN_H_
#define RENDU_TIME_TIME_TIME_SPAN_H_

#include "time_point.h"
RD_TIME_NAMESPACE_BEGIN

class TimeSpan
    : public EqualityComparable<TimeSpan>,
      public LessThanComparable<TimeSpan> {
public:
  using Duration = detail::Milliseconds;

public:
  TimeSpan(Long milliseconds);

  TimeSpan(Int days, Int hours, Long minutes = 0, Long seconds = 0, Long milliseconds = 0);

public:
  Int totalDays() const;
  Long totalHours() const;
  Long totalMinutes() const;
  Long totalSeconds() const;
  Long totalMilliseconds() const;

  Int Days() const;
  Long Hours() const;
  Long Minutes() const;
  Long Seconds() const;
  Long Milliseconds() const;

  TimeSpan operator*(const Long &rhs) const;
  TimeSpan operator/(const Long &rhs) const;
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

private:
  Duration m_duration;
};

RD_TIME_NAMESPACE_END

#include "time_span.inl"

#endif//RENDU_TIME_TIME_TIME_SPAN_H_