/*
* Created by boil on 2024/7/12.
*/

#include "year_month_day_last.h"

#include "date_operators.h"
#include "month_day.h"
#include "year_month.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline YearMonthDayLast::YearMonthDayLast(const Year &y, const MonthDayLast &mdl) noexcept
    : y_(y), mdl_(mdl) {}

template<class>
constexpr inline YearMonthDayLast &YearMonthDayLast::operator+=(const detail::Months &m) noexcept {
  *this = *this + m;
  return *this;
}

template<class>
constexpr inline YearMonthDayLast &YearMonthDayLast::operator-=(const detail::Months &m) noexcept {
  *this = *this - m;
  return *this;
}

constexpr inline YearMonthDayLast &YearMonthDayLast::operator+=(const detail::Years &y) noexcept {
  *this = *this + y;
  return *this;
}

constexpr inline YearMonthDayLast &YearMonthDayLast::operator-=(const detail::Years &y) noexcept {
  *this = *this - y;
  return *this;
}

constexpr inline Year YearMonthDayLast::year() const noexcept { return y_; }
constexpr inline Month YearMonthDayLast::month() const noexcept { return mdl_.month(); }

constexpr inline MonthDayLast YearMonthDayLast::month_day_last() const noexcept {
  return mdl_;
}

constexpr inline Day YearMonthDayLast::day() const noexcept {
  return (Month() != February || !y_.is_leap()) && mdl_.ok() ? DaysInMonth365[static_cast<unsigned>(Month()) - 1] : Day{29};
}

constexpr inline YearMonthDayLast::operator detail::SysDays() const noexcept {
  return detail::SysDays(year() / month() / day());
}

constexpr inline YearMonthDayLast::operator detail::LocalDays() const noexcept {
  return detail::LocalDays(year() / month() / day());
}

constexpr inline bool YearMonthDayLast::ok() const noexcept {
  return y_.ok() && mdl_.ok();
}

constexpr inline bool operator==(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept {
  return x.year() == y.year() && x.month_day_last() == y.month_day_last();
}

constexpr inline bool operator!=(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept {
  return x.year() < y.year() ? true
                             : (x.year() > y.year() ? false
                                                    : (x.month_day_last() < y.month_day_last()));
}

constexpr inline bool operator>(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept {
  return !(x < y);
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &
low_level_fmt(std::basic_ostream<CharT, Traits> &os, const YearMonthDayLast &ymdl) {
  low_level_fmt(os, ymdl.year()) << '/';
  return low_level_fmt(os, ymdl.month_day_last());
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthDayLast &ymdl) {
  detail::low_level_fmt(os, ymdl);
  if (!ymdl.ok())
    os << " is not a valid YearMonthDayLast";
  return os;
}

template<class>
constexpr inline YearMonthDayLast operator+(const YearMonthDayLast &ymdl, const detail::Months &dm) noexcept {
  return (ymdl.year() / ymdl.month() + dm) / last;
}

template<class>
constexpr inline YearMonthDayLast operator+(const detail::Months &dm, const YearMonthDayLast &ymdl) noexcept {
  return ymdl + dm;
}

template<class>
constexpr inline YearMonthDayLast operator-(const YearMonthDayLast &ymdl, const detail::Months &dm) noexcept {
  return ymdl + (-dm);
}

constexpr inline YearMonthDayLast operator+(const YearMonthDayLast &ymdl, const detail::Years &dy) noexcept {
  return {ymdl.year() + dy, ymdl.month_day_last()};
}

constexpr inline YearMonthDayLast operator+(const detail::Years &dy, const YearMonthDayLast &ymdl) noexcept {
  return ymdl + dy;
}

constexpr inline YearMonthDayLast operator-(const YearMonthDayLast &ymdl, const detail::Years &dy) noexcept {
  return ymdl + (-dy);
}



RD_TIME_NAMESPACE_END