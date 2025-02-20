/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_TIME_DATE_TIME_H
#define RENDU_TIME_DATE_TIME_H

#include "date_define.h"
#include "time_duration.h"
#include "time_span.h"
#include "utilities.h"

RD_TIME_NAMESPACE_BEGIN

class DateTime {
public:
  using SysTimePoint = detail::SysTimePoint;
  // Add the Kind enum
  enum class Kind {
    Unspecified,
    Utc,
    Local
  };

  enum class Clock {
    Unspecified,
    System,
    Steady
  };

public:
  static DateTime MinValue;
  static DateTime MaxValue;
  static DateTime UnixEpoch;


public:
  DateTime(Int year, UInt month, UInt day, Kind kind = Kind::Local);
  DateTime(Int year, UInt month, UInt day, Int hour, Int minute, Int second, Int millisecond, Kind kind = Kind::Local);
  DateTime(Long microseconds, Kind kind = Kind::Local);
  DateTime(const SysTimePoint &time_poInt, Kind kind = Kind::Local);

  String ToString(const std::string &format = "%Y-%m-%d %H:%M:%S") const;

public:
  static DateTime Now(Kind kind = Kind::Local);

  bool operator<(const DateTime &other) const;
  bool operator==(const DateTime &other) const;

  DateTime operator+(const TimeSpan &t) const;
  DateTime operator-(const TimeSpan &t) const;

public:
  Int Year() const;
  UInt Month() const;
  UInt Day() const;
  Long Hour() const;
  Long Minute() const;
  Long Second() const;
  Long Millisecond() const;

public:
  DateTime AddYears(Int years) const;
  DateTime AddMonths(Int months) const;
  DateTime AddDays(Int days) const;
  DateTime AddHours(Int years) const;
  DateTime AddMinutes(Long minutes) const;
  DateTime AddSeconds(Long seconds) const;
  DateTime AddMilliseconds(Long milliseconds) const;

  //
  //public:
  //  Int DayOfWeek() const;
  //  Int DayOfYear() const;
  //  bool IsLeapYear() const;

private:
  SysTimePoint m_time_point;
  Kind m_kind;
};


RD_TIME_NAMESPACE_END

#include "date_time.inl"

#endif//RENDU_TIME_DATE_TIME_H
