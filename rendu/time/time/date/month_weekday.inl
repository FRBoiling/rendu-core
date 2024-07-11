/*
* Created by boil on 2024/7/12.
*/

#include "month_weekday.h"

RD_TIME_NAMESPACE_BEGIN
constexpr inline MonthWeekday::MonthWeekday(const Month &m,
                                            const WeekdayIndexed &wdi) noexcept
    : m_(m), wdi_(wdi) {}

constexpr inline Month MonthWeekday::month() const noexcept { return m_; }

constexpr inline WeekdayIndexed MonthWeekday::weekday_indexed() const noexcept {
  return wdi_;
}

constexpr inline bool MonthWeekday::ok() const noexcept {
  return m_.ok() && wdi_.ok();
}

constexpr inline bool operator==(const MonthWeekday &x, const MonthWeekday &y) noexcept {
  return x.month() == y.month() && x.weekday_indexed() == y.weekday_indexed();
}

constexpr inline bool operator!=(const MonthWeekday &x, const MonthWeekday &y) noexcept {
  return !(x == y);
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const MonthWeekday &mwd) {
  low_level_fmt(os, mwd.month()) << '/';
  return low_level_fmt(os, mwd.weekday_indexed());
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthWeekday &mwd) {
  detail::low_level_fmt(os, mwd);
  if (!mwd.ok())
    os << " is not a valid MonthWeekday";
  return os;
}



RD_TIME_NAMESPACE_END
