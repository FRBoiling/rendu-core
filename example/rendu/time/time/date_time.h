/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_TIME_DATE_TIME_H
#define RENDU_TIME_DATE_TIME_H

#include "time_define.h"
#include "time_span.h"

#include <iostream>
#include <sstream>

TIME_NAMESPACE_BEGIN

class DateTime : public Copyable,
                 public LessThanComparable<DateTime>,
                 public EqualityComparable<DateTime> {
public:
  // Add the Kind enum
  enum class Kind {
    Unspecified,
    Utc,
    Local
  };

public:
  static DateTime MinValue;
  static DateTime MaxValue;
  static DateTime UnixEpoch;

public:
  using SysTimePoint = std::chrono::system_clock::time_point;

public:
  DateTime(INT32 year, UINT32 month, UINT32 day, Kind kind = Kind::Local);
  DateTime(INT32 year, UINT32 month, UINT32 day, INT32 hour, INT32 minute, INT32 second, INT32 millisecond, Kind kind = Kind::Local);
  DateTime(const INT64 ticks, Kind kind = Kind::Local);
  DateTime(const SysTimePoint &time_point);

  bool operator<(const DateTime& other) const;
  bool operator==(const DateTime& other) const;

public:
  DateTime AddYears(INT32 years) const;
  DateTime AddMonths(INT32 months) const;
  DateTime AddDays(INT32 days) const;

public:
  INT32 Year() const;
  UINT32 Month() const;
  UINT32 Day() const;
  INT32 Hour() const;
  INT32 Minute() const;
  INT32 Second() const;
  INT64 MilliSecond() const;

public:
  INT32 DayOfWeek() const;
  INT32 DayOfYear() const;
  bool IsLeapYear() const;
  DateTime operator+(const TimeSpan &t) const;
  DateTime operator-(const TimeSpan &t) const;
  STRING ToString(const std::string &format = "%F %T") const;

private:
  void ToKind(Kind kind);
  void ConvertToKind(Kind kind);

  INT32 MilliSecondsInLastDay() const;
  INT64 MilliSecondsInDayParts() const;

public:
  static DateTime Now(const std::string &timezone = "");

  SysTimePoint GetSysTimePoint() const;

private:
  SysTimePoint m_time_point;
  Kind m_kind;
};


TIME_NAMESPACE_END

#endif//RENDU_TIME_DATE_TIME_H
