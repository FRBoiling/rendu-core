/*
* Created by boil on 2024/7/12.
*/

#include "month_day.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline MonthDay::MonthDay(const Month &m, const Day &d) noexcept
    : m_(m), d_(d) {
}

constexpr inline Month MonthDay::month() const noexcept {
  return m_;
}
constexpr inline Day MonthDay::day() const noexcept {
  return d_;
}

constexpr inline bool MonthDay::ok() const noexcept {
  return m_.ok() && Day{1} <= d_ && d_ <= DaysInMonth366[static_cast<unsigned>(m_) - 1];
}

constexpr inline bool operator==(const MonthDay &x, const MonthDay &y) noexcept {
  return x.month() == y.month() && x.day() == y.day();
}

constexpr inline bool operator!=(const MonthDay &x, const MonthDay &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const MonthDay &x, const MonthDay &y) noexcept {
  return x.month() < y.month() ? true
                               : (x.month() > y.month() ? false
                                                        : (x.day() < y.day()));
}

constexpr inline bool operator>(const MonthDay &x, const MonthDay &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const MonthDay &x, const MonthDay &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const MonthDay &x, const MonthDay &y) noexcept {
  return !(x < y);
}

RD_DETAIL_NAMESPACE_BEGIN
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const MonthDay &md) {
  low_level_fmt(os, md.month()) << '/';
  return low_level_fmt(os, md.day());
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthDay &md) {
  detail::low_level_fmt(os, md);
  if (!md.ok())
    os << " is not a valid MonthDay";
  return os;
}

RD_TIME_NAMESPACE_END