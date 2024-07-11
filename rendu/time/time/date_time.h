/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_TIME_DATE_TIME_H
#define RENDU_TIME_DATE_TIME_H

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

public:
  static DateTime MinValue;
  static DateTime MaxValue;
  static DateTime UnixEpoch;


public:
  DateTime(Int year, UInt month, UInt day, Kind kind = Kind::Local);
  DateTime(Int year, UInt month, UInt day, Int hour, Int minute, Int second, Int millisecond, Kind kind = Kind::Local);
  DateTime(const Long ticks, Kind kind = Kind::Local);
  DateTime(const SysTimePoint &time_poInt);

  String ToString(const std::string &format = "%F %T") const;

public:
  static DateTime Now(const std::string &timezone = "");

  //  bool operator<(const DateTime &other) const;
  //  bool operator==(const DateTime &other) const;
  //
  //public:
  //  DateTime AddYears(Int years) const;
  //  DateTime AddMonths(Int months) const;
  //  DateTime AddDays(Int days) const;
  //
  //public:
  //  Int Year() const;
  //  UInt Month() const;
  //  UInt Day() const;
  //  Int Hour() const;
  //  Int Minute() const;
  //  Int Second() const;
  //  Long MilliSecond() const;
  //
  //public:
  //  Int DayOfWeek() const;
  //  Int DayOfYear() const;
  //  bool IsLeapYear() const;
  //  DateTime operator+(const TimeSpan &t) const;
  //  DateTime operator-(const TimeSpan &t) const;
  //
  //private:
  //  void ToKind(Kind kind);
  //  void ConvertToKind(Kind kind);
  //
  //  Int MilliSecondsInLastDay() const;
  //  Long MilliSecondsInDayParts() const;
  //
  //


private:
  SysTimePoint m_time_point;
  Kind m_kind;
};


template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const DateTime &dt) {
  detail::save_ostream<CharT, Traits> _(os);
  os << dt.ToString();
  return os;
}

//
/**
 * 获取当前的ms信息
 * @return
 */
inline MSec RD_GET_CURRENT_MS() {
  // 获取当前的时间戳信息
  return (MSec) std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
}


/**
 * 获取当前的ms信息(包含小数)
 * @return
 */
inline FMSec RD_GET_CURRENT_ACCURATE_MS() {
  // 获取当前的时间戳信息
  return (FMSec) std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count() / (FMSec) 1000.0;
}

RD_TIME_NAMESPACE_END

#endif//RENDU_TIME_DATE_TIME_H
