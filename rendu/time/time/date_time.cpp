/*
* Created by boil on 2023/9/22.
*/

#include "date_time.h"

RD_TIME_NAMESPACE_BEGIN

DateTime DateTime::MinValue{0};
DateTime DateTime::MaxValue{detail::MaxMicroseconds, DateTime::Kind::Unspecified};
DateTime DateTime::UnixEpoch{detail::UnixEpochMicros, DateTime::Kind::Utc};


DateTime::DateTime(Int year, uint month, uint day, DateTime::Kind kind)
    : m_time_point{detail::SysDays{detail::YearMonthDay{detail::Year{year}, detail::Month{month}, detail::Day{day}}}},
      m_kind{kind} {
  //  ConvertToKind(m_kind);
}

DateTime::DateTime(Int year, uint month, uint day, Int hour, Int minute, Int second, Int millisecond, DateTime::Kind kind)
    : m_time_point{detail::SysDays{detail::YearMonthDay{detail::Year{year}, detail::Month{month}, detail::Day{day}}} + detail::Hours(hour) + detail::Minutes(minute) + detail::Seconds(second) + detail::Milliseconds(millisecond)},
      m_kind{kind} {
  //  ConvertToKind(m_kind);
}

DateTime::DateTime(Long microseconds, DateTime::Kind kind)
    : m_time_point{detail::SysDays{detail::YearMonthDay{detail::Year{0}, detail::Month{0}, detail::Day{0}}} + detail::Hours(0) + detail::Minutes(0) + detail::Seconds(0) + detail::Milliseconds(0) + detail::Microseconds(microseconds)},
      m_kind{kind} {
}

DateTime::DateTime(const DateTime::SysTimePoint &time_point, DateTime::Kind kind)
    : m_time_point(time_point),
      m_kind(kind) {
}

String DateTime::ToString(const std::string &format /*= "%F %T %3f"*/) const {
  if (m_kind == Kind::Unspecified) {
    return "";
  }
  if (m_kind == Kind::Utc) {
    std::stringstream ss;
    ss << m_time_point;
    return ss.str();
  }

  // 将 time_point 转换为 time_t
  std::time_t tt = std::chrono::system_clock::to_time_t(m_time_point);
  // 将 time_t 转换为本地时间的 tm 结构体
  std::tm *tm = std::localtime(&tt);
  std::ostringstream oss;
  oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S.");

  detail::Milliseconds ms = duration_cast<detail::Milliseconds>(m_time_point.time_since_epoch()) % 1000000;
  oss << std::setfill('0') << std::setw(3) << ms.count() % 1000;
  // 格式化本地时间为字符串
  return oss.str();
}

DateTime DateTime::Now(Kind kind /*= Kind::Local */) {
  auto now = detail::SysClock::now();
  return {now, kind};
}

DateTime DateTime::AddYears(Int years) const {
  auto dt = floor<detail::Days>(m_time_point);
  auto ymd = detail::YearMonthDay{dt} + detail::Years(years);
  auto time_of_day = m_time_point - dt;
  return {SysTimePoint(detail::SysDays{ymd}) + time_of_day, m_kind};
}

DateTime DateTime::AddMonths(Int months) const {
  auto dt = floor<detail::Days>(m_time_point);
  auto ymd = detail::YearMonthDay{dt} + detail::Months(months);
  auto time_of_day = m_time_point - dt;
  return {SysTimePoint(detail::SysDays{ymd}) + time_of_day, m_kind};
}

DateTime DateTime::AddDays(Int days) const {
  //  return {m_time_point + detail::Days(days), m_kind};
  TimeSpan ts{days, 0};
  return *this + ts;
}

DateTime DateTime::AddHours(Int hours) const {
  //  return {m_time_point + detail::Hours(hours), m_kind};
  TimeSpan ts{0, hours};
  return *this + ts;
}

DateTime DateTime::AddMinutes(Long minutes) const {
  //  return {m_time_point + detail::Minutes(minutes), m_kind};
  TimeSpan ts{0, 0, minutes};
  return *this + ts;
}

DateTime DateTime::AddSeconds(Long seconds) const {
  //  return {m_time_point + detail::Seconds(seconds), m_kind};
  TimeSpan ts{0, 0, 0, seconds};
  return *this + ts;
}

DateTime DateTime::AddMilliseconds(Long milliseconds) const {
  //  return {m_time_point + detail::Milliseconds(milliseconds)};
  TimeSpan ts{0, 0, 0, 0, milliseconds};
  return *this + ts;
}

//
//bool DateTime::IsLeapYear() const {
//  return Year_month_day{date::floor<date::days>(m_time_point)}.year().is_leap();
//}
//
//Int DateTime::DayOfYear() const {
//  return static_cast<Int>(date::floor<date::days>(m_time_point).time_since_epoch().count() -
//                            detail::SysDays(Year{Year()} / 1 / 1).time_since_epoch().count() + 1);
//}
//Int DateTime::DayOfWeek() const {
//  return (Int) (date::weekday{date::year_month_day{date::floor<date::days>(m_time_point)}}.iso_encoding());
//}


//

//
RD_TIME_NAMESPACE_END
