/*
* Created by boil on 2023/9/22.
*/

#include "date_time.h"
#include "date/tz.h"

#include <chrono>
#include <iomanip>
#include <utility>

TIME_NAMESPACE_BEGIN

DateTime DateTime::MinValue{0};
DateTime DateTime::MaxValue{MaxTicks, DateTime::Kind::Unspecified};
DateTime DateTime::UnixEpoch{UnixEpochTicks, DateTime::Kind::Utc};

void DateTime::ToKind(time::DateTime::Kind kind) {
  using namespace std::chrono;

  if (m_kind == kind) return;

  auto tp_sys = time_point_cast<milliseconds>(m_time_point);
  auto tp_sys_time_t = system_clock::to_time_t(time_point_cast<seconds>(tp_sys));// Convert to seconds to feed to localtime/gmtime

  // Extract the milliseconds part
  auto ms_part = duration_cast<milliseconds>(tp_sys.time_since_epoch()).count() % 1000;

  std::tm result = {};
  std::tm *tm = &result;
  if (kind == Kind::Local) {
    localtime_r(&tp_sys_time_t, tm);
  } else if (kind == Kind::Utc) {
    gmtime_r(&tp_sys_time_t, tm);
  }

  if (!tm) {
    throw std::runtime_error("Fail to convert to time kind");
  }

  tm->tm_isdst = 0;
  using namespace date;
  auto dp = date::year{tm->tm_year + 1900} / date::month{tm->tm_mon + 1} / date::day{tm->tm_mday};// From struct tm to date::year_month_day
  auto t = hours{tm->tm_hour} + minutes{tm->tm_min} + seconds{tm->tm_sec};                        // From struct tm to std::chrono::*
  auto tp2 = sys_days(dp) + t + milliseconds(ms_part);

  m_time_point = SysTimePoint(duration_cast<milliseconds>(tp2.time_since_epoch()));
  m_kind = kind;
}

void DateTime::ConvertToKind(DateTime::Kind kind) {
  if (kind != Kind::Unspecified) {
    this->ToKind(kind);
  }
}


DateTime::DateTime(INT32 year, UINT32 month, UINT32 day, DateTime::Kind kind)
    : m_kind(kind),
      m_time_point(date::sys_days(date::year{year} / date::month{month} / date::day{day})) {
  ConvertToKind(m_kind);
}

DateTime::DateTime(INT32 year, UINT32 month, UINT32 day, INT32 hour, INT32 minute, INT32 second, INT32 millisecond, DateTime::Kind kind)
    : m_kind(kind),
      m_time_point(date::sys_days(date::year{year} / date::month{month} / date::day{day}) + std::chrono::hours(hour) + std::chrono::minutes(minute) + std::chrono::seconds(second) + std::chrono::milliseconds(millisecond)) {
  ConvertToKind(m_kind);
}

DateTime::DateTime(const INT64 ticks, DateTime::Kind kind)
    : m_kind(kind),
      m_time_point(std::chrono::milliseconds(ticks)) {
  ConvertToKind(m_kind);
}

DateTime::DateTime(const DateTime::SysTimePoint &time_point) : m_time_point(time_point) {}

DateTime DateTime::Now(const std::string &timezone /*= ""*/) {
  auto now = std::chrono::system_clock::now();
  if (!timezone.empty()) {
    auto zoned_time = date::make_zoned(timezone, now);
    return DateTime(std::chrono::system_clock::from_time_t(date::floor<std::chrono::seconds>(zoned_time.get_local_time()).time_since_epoch().count()));
  } else {
    return DateTime(now);
  }
}

DateTime::SysTimePoint DateTime::GetSysTimePoint() const {
  return m_time_point;
}

//std::string DateTime::ToString(const std::string& format /*= "%F %T"*/) const {
//  auto tp = floor<milliseconds>(m_time_point);
//  auto ms = tp.time_since_epoch().count() % 1000;
//  std::string base_date_time = date::format(format, tp);
//
//  std::ostringstream oss;
//  oss << std::setfill('0') << std::setw(3) << ms; //添加毫秒并确保毫秒值为三位数
//
//  return base_date_time + "." + oss.str(); //拼接字符串
//}

STRING DateTime::ToString(const std::string &format /*= "%F %T %3f"*/) const {
  return date::format(format, date::floor<std::chrono::milliseconds>(m_time_point));
}

DateTime DateTime::operator-(const TimeSpan &t) const {
  auto duration = std::chrono::duration<UINT64, std::milli>(t.TotalMilliseconds());
  return DateTime(m_time_point - duration);
}
DateTime DateTime::operator+(const TimeSpan &t) const {
  auto duration = std::chrono::duration<long long, std::milli>(t.TotalMilliseconds());
  return DateTime(m_time_point + duration);
}

bool DateTime::IsLeapYear() const {
  return date::year_month_day{date::floor<date::days>(m_time_point)}.year().is_leap();
}

INT32 DateTime::DayOfYear() const {
  return static_cast<INT32>(date::floor<date::days>(m_time_point).time_since_epoch().count() -
                            date::sys_days(date::year{Year()} / 1 / 1).time_since_epoch().count() + 1);
}
INT32 DateTime::DayOfWeek() const {
  return (INT32) (date::weekday{date::year_month_day{date::floor<date::days>(m_time_point)}}.iso_encoding());
}

INT32 DateTime::Year() const {
  return (INT32) (date::year_month_day{date::floor<date::days>(m_time_point)}.year());
}

UINT32 DateTime::Month() const {
  return (UINT32) (date::year_month_day{date::floor<date::days>(m_time_point)}.month());
}

UINT32 DateTime::Day() const {
  return (UINT32) (date::year_month_day{date::floor<date::days>(m_time_point)}.day());
}

// MilliSecondsInLastDay函数计算自午夜以来的毫秒数
// 返回值的范围是0到8639999
// 8639999毫秒是23小时59分钟59秒
// 23小时59分钟59秒是24小时中的1秒
// 因此，返回值的范围是从0到8639999，表示自午夜以来的毫秒数
INT32 DateTime::MilliSecondsInLastDay() const {
  const auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(m_time_point.time_since_epoch()).count();
  return static_cast<INT32>(seconds_since_epoch % MillisPerDay);
}

INT32 DateTime::Hour() const {
  return date::make_time(std::chrono::milliseconds(MilliSecondsInLastDay())).hours().count();
}

INT32 DateTime::Minute() const {
  return date::make_time(std::chrono::milliseconds(MilliSecondsInLastDay())).minutes().count();
}

INT32 DateTime::Second() const {
  return date::make_time(std::chrono::milliseconds(MilliSecondsInLastDay())).seconds().count();
}

// MilliSecondsInDayParts函数计算表示小时、分钟和秒的毫秒数总和，这将用于计算剩余毫秒数
INT64 DateTime::MilliSecondsInDayParts() const {
  auto hms_ms = std::chrono::milliseconds(MilliSecondsInLastDay());
  date::hh_mm_ss hms(hms_ms);
  return INT64(hms.hours().count()) * MillisPerHour + INT64(hms.minutes().count()) * MillisPerMinute + INT64(hms.seconds().count()) * MillisPerSecond;
}

INT64 DateTime::MilliSecond() const {
  return std::chrono::milliseconds(MilliSecondsInLastDay()).count() - MilliSecondsInDayParts();
}

DateTime DateTime::AddYears(INT32 years) const {
  auto dt = date::floor<date::days>(m_time_point);
  auto ymd = date::year_month_day{dt} + date::years(years);
  auto time_of_day = m_time_point - dt;
  return DateTime(SysTimePoint(date::sys_days{ymd}) + time_of_day);
}

DateTime DateTime::AddMonths(INT32 months) const {
  auto dt = date::floor<date::days>(m_time_point);
  auto ymd = date::year_month_day{dt} + date::months(months);
  auto time_of_day = m_time_point - dt;
  return DateTime(SysTimePoint(date::sys_days{ymd}) + time_of_day);
}

DateTime DateTime::AddDays(INT32 days) const {
  return DateTime(m_time_point + date::days{days});
}

bool DateTime::operator==(const DateTime &other) const {
  return m_time_point == other.m_time_point;
}
bool DateTime::operator<(const DateTime &other) const {
  return m_time_point < other.m_time_point;
}


TIME_NAMESPACE_END
