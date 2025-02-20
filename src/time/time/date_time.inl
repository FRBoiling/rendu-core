/*
* Created by boil on 2023/9/22.
*/

#include "date_define.h"
#include "date_time.h"

RD_TIME_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const DateTime &dt) {
  detail::save_ostream<CharT, Traits> _(os);
  os << dt.ToString();
  return os;
}

inline DateTime DateTime::operator-(const TimeSpan &t) const {
  auto duration = TimeSpan::Duration(t.totalMilliseconds());
  return {m_time_point - duration,m_kind};
}

inline DateTime DateTime::operator+(const TimeSpan &t) const {
  auto duration = TimeSpan::Duration(t.totalMilliseconds());
  return {m_time_point + duration,m_kind};
}


inline bool DateTime::operator==(const DateTime &other) const {
  return m_time_point == other.m_time_point;
}
inline bool DateTime::operator<(const DateTime &other) const {
  return m_time_point < other.m_time_point;
}


inline Int DateTime::Year() const {
  //  auto tp = zoned_time{current_zone(), system_clock::now()}.get_local_time();
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::year_month_day ymd{dp};
  auto y = ymd.year();
  // 返回年份
  return (Int)y;
}

inline UInt DateTime::Month() const {
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::year_month_day ymd{dp};
  auto m = ymd.month();
  return (UInt)m;
}

inline UInt DateTime::Day() const {
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::year_month_day ymd{dp};
  auto d = ymd.day();
  return (UInt)d;
}

inline Long DateTime::Hour() const {
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::hh_mm_ss time{floor<TimeSpan::Duration>(m_time_point-dp)};
  auto h = time.hours();
  return h.count();
}

inline Long DateTime::Minute() const {
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::year_month_day ymd{dp};
  std::chrono::hh_mm_ss time{floor<TimeSpan::Duration>(m_time_point-dp)};
  auto M = time.minutes();
  return M.count();
}

inline Long DateTime::Second() const {
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::hh_mm_ss time{floor<TimeSpan::Duration>(m_time_point-dp)};
  auto s = time.seconds();
  return s.count();
}
//
inline Long DateTime::Millisecond() const {
  auto dp = floor<detail::Days>(m_time_point);
  std::chrono::hh_mm_ss time{floor<TimeSpan::Duration>(m_time_point-dp)};
  auto ms = time.subseconds();
  return ms.count();
}



RD_TIME_NAMESPACE_END

