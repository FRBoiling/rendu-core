/*
* Created by boil on 2024/7/12.
*/

#include "weekday_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline Weekday WeekdayLast::weekday() const noexcept { return wd_; }
constexpr inline bool WeekdayLast::ok() const noexcept { return wd_.ok(); }
constexpr inline WeekdayLast::WeekdayLast(const Weekday &wd) noexcept : wd_(wd) {}

constexpr inline bool operator==(const WeekdayLast &x, const WeekdayLast &y) noexcept {
  return x.weekday() == y.weekday();
}

constexpr inline bool operator!=(const WeekdayLast &x, const WeekdayLast &y) noexcept {
  return !(x == y);
}

RD_DETAIL_NAMESPACE_BEGIN
template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const WeekdayLast &wdl) {
  return low_level_fmt(os, wdl.weekday()) << "[last]";
}
RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> &os, const WeekdayLast &wdl) {
  detail::low_level_fmt(os, wdl);
  if (!wdl.ok())
    os << " is not a valid WeekdayLast";
  return os;
}

constexpr inline WeekdayLast Weekday::operator[](LastSpec) const noexcept {
  return WeekdayLast{*this};
}

RD_TIME_NAMESPACE_END