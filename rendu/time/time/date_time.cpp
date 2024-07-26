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
  return {now,kind};
}

DateTime DateTime::AddYears(Int years) const {

  auto dt = floor<detail::Days>(m_time_point);
  auto ymd = detail::YearMonthDay{dt} + detail::Years(years);
  auto time_of_day = m_time_point - dt;
  return {SysTimePoint(detail::SysDays{ymd}) + time_of_day,m_kind};
}

DateTime DateTime::AddMonths(Int months) const {
  auto dt = floor<detail::Days>(m_time_point);
  auto ymd = detail::YearMonthDay{dt} + detail::Months(months);
  auto time_of_day = m_time_point - dt;
  return {SysTimePoint(detail::SysDays{ymd}) + time_of_day,m_kind};
}

DateTime DateTime::AddDays(Int days) const {
  return {m_time_point + detail::Days(days), m_kind};
}

DateTime DateTime::AddHours(Int hours) const {
  return {m_time_point + detail::Hours(hours),m_kind};
}

DateTime DateTime::AddMinutes(Long minutes) const {
  return {m_time_point + detail::Minutes(minutes),m_kind};
}

DateTime DateTime::AddSeconds(Long seconds) const {
  return {m_time_point + detail::Seconds(seconds),m_kind};
}

DateTime DateTime::AddMilliseconds(Long milliseconds) const {
  return {m_time_point + detail::Milliseconds(milliseconds)};
}


//void DateTime::ToKind(time::DateTime::Kind kind) {
////  using namespace std::chrono;
////
////  if (m_kind == kind) return;
////
////  auto tp_sys = time_point_cast<detail::Milliseconds>(m_time_point);
////  auto tp_sys_time_t = system_clock::to_time_t(time_point_cast<seconds>(tp_sys));// Convert to seconds to feed to localtime/gmtime
////
////  // Extract the milliseconds part
////  auto ms_part = duration_cast<milliseconds>(tp_sys.time_since_epoch()).count() % 1000;
////
////  std::tm result = {};
////  std::tm *tm = &result;
////  if (kind == Kind::Local) {
////    localtime_r(&tp_sys_time_t, tm);
////  } else if (kind == Kind::Utc) {
////    gmtime_r(&tp_sys_time_t, tm);
////  }
////
////  if (!tm) {
////    throw std::runtime_error("Fail to convert to time kind");
////  }
////
////  tm->tm_isdst = 0;
////  using namespace date;
////  auto dp = date::year{tm->tm_year + 1900} / date::month{tm->tm_mon + 1} / date::day{tm->tm_mday};// From struct tm to date::year_month_day
////  auto t = hours{tm->tm_hour} + minutes{tm->tm_min} + seconds{tm->tm_sec};                        // From struct tm to std::chrono::*
////  auto tp2 = sys_days(dp) + t + milliseconds(ms_part);
////
////  m_time_point = SysTimePoint(duration_cast<milliseconds>(tp2.time_since_epoch()));
////  m_kind = kind;
//}
//
//void DateTime::ConvertToKind(DateTime::Kind kind) {
//  if (kind != Kind::Unspecified) {
//    this->ToKind(kind);
//  }
//}


//
//DateTime::SysTimePoint DateTime::GetSysTimePoint() const {
//  return m_time_point;
//}
//
////std::string DateTime::ToString(const std::string& format /*= "%F %T"*/) const {
////  auto tp = floor<milliseconds>(m_time_point);
////  auto ms = tp.time_since_epoch().count() % 1000;
////  std::string base_date_time = date::format(format, tp);
////
////  std::ostringstream oss;
////  oss << std::setfill('0') << std::setw(3) << ms; //添加毫秒并确保毫秒值为三位数
////
////  return base_date_time + "." + oss.str(); //拼接字符串
////}
//

//

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
