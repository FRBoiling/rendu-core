/*
* Created by boil on 2024/7/12.
*/

#include "year_month_weekday.h"

#include "date_operators.h"
#include "year_month_day_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline YearMonthWeekday::YearMonthWeekday(const Year &y, const Month &m, const WeekdayIndexed &wdi) noexcept
    : y_(y), m_(m), wdi_(wdi) {}

constexpr inline YearMonthWeekday::YearMonthWeekday(const detail::SysDays &dp) noexcept
    : YearMonthWeekday(from_days(dp.time_since_epoch())) {}

constexpr inline YearMonthWeekday::YearMonthWeekday(const detail::LocalDays &dp) noexcept
    : YearMonthWeekday(from_days(dp.time_since_epoch())) {}

template<class>
constexpr inline YearMonthWeekday &YearMonthWeekday::operator+=(const detail::Months &m) noexcept {
  *this = *this + m;
  return *this;
}

template<class>
constexpr inline YearMonthWeekday &YearMonthWeekday::operator-=(const detail::Months &m) noexcept {
  *this = *this - m;
  return *this;
}

constexpr inline YearMonthWeekday &YearMonthWeekday::operator+=(const detail::Years &y) noexcept {
  *this = *this + y;
  return *this;
}

constexpr inline YearMonthWeekday &YearMonthWeekday::operator-=(const detail::Years &y) noexcept {
  *this = *this - y;
  return *this;
}

constexpr inline Year YearMonthWeekday::year() const noexcept { return y_; }
constexpr inline Month YearMonthWeekday::month() const noexcept { return m_; }

constexpr inline Weekday YearMonthWeekday::weekday() const noexcept {
  return wdi_.weekday();
}

constexpr inline unsigned YearMonthWeekday::index() const noexcept {
  return wdi_.index();
}

constexpr inline WeekdayIndexed YearMonthWeekday::weekday_indexed() const noexcept {
  return wdi_;
}

constexpr inline YearMonthWeekday::operator detail::SysDays() const noexcept {
  return detail::SysDays{to_days()};
}

constexpr inline YearMonthWeekday::operator detail::LocalDays() const noexcept {
  return detail::LocalDays{to_days()};
}

constexpr inline bool YearMonthWeekday::ok() const noexcept {
  if (!y_.ok() || !m_.ok() || !wdi_.weekday().ok() || wdi_.index() < 1)
    return false;
  if (wdi_.index() <= 4)
    return true;
  auto d2 = wdi_.weekday() - Weekday(static_cast<detail::SysDays>(y_ / m_ / 1)) +
            detail::Days((wdi_.index() - 1) * 7 + 1);
  return static_cast<unsigned>(d2.count()) <= static_cast<unsigned>((y_ / m_ / last).day());
}

constexpr inline YearMonthWeekday YearMonthWeekday::from_days(detail::Days d) noexcept {
  detail::SysDays dp{d};
  auto const wd = Weekday(dp);
  auto const ymd = YearMonthDay(dp);
  return {ymd.year(), ymd.month(), wd[(static_cast<unsigned>(ymd.day()) - 1) / 7 + 1]};
}

constexpr inline detail::Days YearMonthWeekday::to_days() const noexcept {
  auto d = detail::SysDays(y_ / m_ / 1);
  return (d + (wdi_.weekday() - Weekday(d) + detail::Days{(wdi_.index() - 1) * 7})).time_since_epoch();
}

constexpr inline bool operator==(const YearMonthWeekday &x, const YearMonthWeekday &y) noexcept {
  return x.year() == y.year() && x.month() == y.month() &&
         x.weekday_indexed() == y.weekday_indexed();
}

constexpr inline bool operator!=(const YearMonthWeekday &x, const YearMonthWeekday &y) noexcept {
  return !(x == y);
}

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthWeekday &ymwdi) {
  detail::low_level_fmt(os, ymwdi.year()) << '/';
  detail::low_level_fmt(os, ymwdi.month()) << '/';
  detail::low_level_fmt(os, ymwdi.weekday_indexed());
  if (!ymwdi.ok())
    os << " is not a valid YearMonthWeekday";
  return os;
}

template<class>
constexpr inline YearMonthWeekday operator+(const YearMonthWeekday &ymwd, const detail::Months &dm) noexcept {
  return (ymwd.year() / ymwd.month() + dm) / ymwd.weekday_indexed();
}

template<class>
constexpr inline YearMonthWeekday operator+(const detail::Months &dm, const YearMonthWeekday &ymwd) noexcept {
  return ymwd + dm;
}

template<class>
constexpr inline YearMonthWeekday operator-(const YearMonthWeekday &ymwd, const detail::Months &dm) noexcept {
  return ymwd + (-dm);
}

constexpr inline YearMonthWeekday operator+(const YearMonthWeekday &ymwd, const detail::Years &dy) noexcept {
  return {ymwd.year() + dy, ymwd.month(), ymwd.weekday_indexed()};
}

constexpr inline YearMonthWeekday operator+(const detail::Years &dy, const YearMonthWeekday &ymwd) noexcept {
  return ymwd + dy;
}

constexpr inline YearMonthWeekday operator-(const YearMonthWeekday &ymwd, const detail::Years &dy) noexcept {
  return ymwd + (-dy);
}



RD_TIME_NAMESPACE_END