/*
* Created by boil on 2024/7/12.
*/

#include "month_weekday_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline MonthWeekdayLast::MonthWeekdayLast(const Month &m,
                                                    const WeekdayLast &wdl) noexcept
    : m_(m), wdl_(wdl) {}

constexpr inline Month MonthWeekdayLast::month() const noexcept { return m_; }

constexpr inline WeekdayLast MonthWeekdayLast::weekday_last() const noexcept {
  return wdl_;
}

constexpr inline bool MonthWeekdayLast::ok() const noexcept {
  return m_.ok() && wdl_.ok();
}

constexpr inline bool operator==(const MonthWeekdayLast &x, const MonthWeekdayLast &y) noexcept {
  return x.month() == y.month() && x.weekday_last() == y.weekday_last();
}

constexpr inline bool operator!=(const MonthWeekdayLast &x, const MonthWeekdayLast &y) noexcept {
  return !(x == y);
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const MonthWeekdayLast &mwdl) {
  low_level_fmt(os, mwdl.month()) << '/';
  return low_level_fmt(os, mwdl.weekday_last());
}
RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthWeekdayLast &mwdl) {
  detail::low_level_fmt(os, mwdl);
  if (!mwdl.ok())
    os << " is not a valid MonthWeekdayLast";
  return os;
}



RD_TIME_NAMESPACE_END