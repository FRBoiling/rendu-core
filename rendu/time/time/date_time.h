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
  DateTime(int year, uint month, uint day, Kind kind = Kind::Local);
  DateTime(int year, uint month, uint day, int hour, int minute, int second, int millisecond, Kind kind = Kind::Local);
  DateTime(const Long ticks, Kind kind = Kind::Local);
  DateTime(const SysTimePoint &time_point);

  bool operator<(const DateTime& other) const;
  bool operator==(const DateTime& other) const;

public:
  DateTime AddYears(int years) const;
  DateTime AddMonths(int months) const;
  DateTime AddDays(int days) const;

public:
  int Year() const;
  uint Month() const;
  uint Day() const;
  int Hour() const;
  int Minute() const;
  int Second() const;
  Long MilliSecond() const;

public:
  int DayOfWeek() const;
  int DayOfYear() const;
  bool IsLeapYear() const;
  DateTime operator+(const TimeSpan &t) const;
  DateTime operator-(const TimeSpan &t) const;
  string ToString(const std::string &format = "%F %T") const;

private:
  void ToKind(Kind kind);
  void ConvertToKind(Kind kind);

  int MilliSecondsInLastDay() const;
  Long MilliSecondsInDayParts() const;

public:
  static DateTime Now(const std::string &timezone = "");

  SysTimePoint GetSysTimePoint() const;

private:
  SysTimePoint m_time_point;
  Kind m_kind;
};


TIME_NAMESPACE_END

#endif//RENDU_TIME_DATE_TIME_H
