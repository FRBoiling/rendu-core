/*
* Created by boil on 2024/7/12.
*/

#include "year_month_weekday_last.h"


#include "date_operators.h"
#include "year_month_day_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline YearMonthWeekdayLast::YearMonthWeekdayLast(const Year &y, const Month &m, const WeekdayLast &wdl) noexcept
    : y_(y), m_(m), wdl_(wdl) {}

template<class>
constexpr inline YearMonthWeekdayLast &YearMonthWeekdayLast::operator+=(const detail::Months &m) noexcept {
  *this = *this + m;
  return *this;
}

template<class>
constexpr inline YearMonthWeekdayLast &YearMonthWeekdayLast::operator-=(const detail::Months &m) noexcept {
  *this = *this - m;
  return *this;
}

constexpr inline YearMonthWeekdayLast &YearMonthWeekdayLast::operator+=(const detail::Years &y) noexcept {
  *this = *this + y;
  return *this;
}

constexpr inline YearMonthWeekdayLast &YearMonthWeekdayLast::operator-=(const detail::Years &y) noexcept {
  *this = *this - y;
  return *this;
}

constexpr inline Year YearMonthWeekdayLast::year() const noexcept { return y_; }
constexpr inline Month YearMonthWeekdayLast::month() const noexcept { return m_; }

constexpr inline Weekday YearMonthWeekdayLast::weekday() const noexcept {
  return wdl_.weekday();
}

constexpr inline WeekdayLast YearMonthWeekdayLast::weekday_last() const noexcept {
  return wdl_;
}

constexpr inline YearMonthWeekdayLast::operator detail::SysDays() const noexcept {
  return detail::SysDays{to_days()};
}

constexpr inline YearMonthWeekdayLast::operator detail::LocalDays() const noexcept {
  return detail::LocalDays{to_days()};
}

constexpr inline bool YearMonthWeekdayLast::ok() const noexcept {
  return y_.ok() && m_.ok() && wdl_.ok();
}

constexpr inline detail:: Days YearMonthWeekdayLast::to_days() const noexcept {
  auto const d = detail::SysDays(y_ / m_ / last);
  return (d - (Weekday{d} - wdl_.weekday())).time_since_epoch();
}

constexpr inline bool operator==(const YearMonthWeekdayLast &x, const YearMonthWeekdayLast &y) noexcept {
  return x.year() == y.year() && x.month() == y.month() &&
         x.weekday_last() == y.weekday_last();
}

constexpr inline bool operator!=(const YearMonthWeekdayLast &x, const YearMonthWeekdayLast &y) noexcept {
  return !(x == y);
}

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthWeekdayLast &ymwdl) {
  detail::low_level_fmt(os, ymwdl.year()) << '/';
  detail::low_level_fmt(os, ymwdl.month()) << '/';
  detail::low_level_fmt(os, ymwdl.weekday_last());
  if (!ymwdl.ok())
    os << " is not a valid YearMonthWeekdayLast";
  return os;
}

template<class>
constexpr inline YearMonthWeekdayLast operator+(const YearMonthWeekdayLast &ymwdl, const detail::Months &dm) noexcept {
  return (ymwdl.year() / ymwdl.month() + dm) / ymwdl.weekday_last();
}

template<class>
constexpr inline YearMonthWeekdayLast operator+(const detail::Months &dm, const YearMonthWeekdayLast &ymwdl) noexcept {
  return ymwdl + dm;
}

template<class>
constexpr inline YearMonthWeekdayLast operator-(const YearMonthWeekdayLast &ymwdl, const detail::Months &dm) noexcept {
  return ymwdl + (-dm);
}

constexpr inline YearMonthWeekdayLast operator+(const YearMonthWeekdayLast &ymwdl, const detail::Years &dy) noexcept {
  return {ymwdl.year() + dy, ymwdl.month(), ymwdl.weekday_last()};
}

constexpr inline YearMonthWeekdayLast operator+(const detail::Years &dy, const YearMonthWeekdayLast &ymwdl) noexcept {
  return ymwdl + dy;
}

constexpr inline YearMonthWeekdayLast operator-(const YearMonthWeekdayLast &ymwdl, const detail::Years &dy) noexcept {
  return ymwdl + (-dy);
}



RD_TIME_NAMESPACE_END