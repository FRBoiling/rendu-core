/*
* Created by boil on 2024/7/12.
*/

#include "month_day_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline Month MonthDayLast::month() const noexcept { return m_; }
constexpr inline bool MonthDayLast::ok() const noexcept { return m_.ok(); }
constexpr inline MonthDayLast::MonthDayLast(const Month &m) noexcept : m_(m) {}

constexpr inline bool operator==(const MonthDayLast &x, const MonthDayLast &y) noexcept {
  return x.month() == y.month();
}

constexpr inline bool operator!=(const MonthDayLast &x, const MonthDayLast &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const MonthDayLast &x, const MonthDayLast &y) noexcept {
  return x.month() < y.month();
}

constexpr inline bool operator>(const MonthDayLast &x, const MonthDayLast &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const MonthDayLast &x, const MonthDayLast &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const MonthDayLast &x, const MonthDayLast &y) noexcept {
  return !(x < y);
}

RD_DETAIL_NAMESPACE_BEGIN

  template<class CharT, class Traits>
  std::basic_ostream<CharT, Traits> &   low_level_fmt(std::basic_ostream<CharT, Traits> &os, const MonthDayLast &mdl) {
    return low_level_fmt(os, mdl.month()) << "/last";
  }

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthDayLast &mdl) {
  detail::low_level_fmt(os, mdl);
  if (!mdl.ok())
    os << " is not a valid MonthDayLast";
  return os;
}



RD_TIME_NAMESPACE_END