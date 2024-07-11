/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_DAY_LAST_H_
#define RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_DAY_LAST_H_

#include "year.h"
#include "month_day_last.h"

RD_TIME_NAMESPACE_BEGIN

class YearMonthDayLast {
private:
  Year y_;
  MonthDayLast mdl_;

public:
  constexpr YearMonthDayLast(const Year &y, const MonthDayLast &mdl) noexcept;

  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthDayLast &operator+=(const detail::Months &m) noexcept;
  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthDayLast &operator-=(const detail::Months &m) noexcept;

  constexpr YearMonthDayLast &operator+=(const detail::Years &y) noexcept;
  constexpr YearMonthDayLast &operator-=(const detail::Years &y) noexcept;

  constexpr Year year() const noexcept;
  constexpr Month month() const noexcept;
  constexpr MonthDayLast month_day_last() const noexcept;
  constexpr Day day() const noexcept;

  constexpr operator detail::SysDays() const noexcept;
  constexpr explicit operator detail::LocalDays() const noexcept;
  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept;
constexpr bool operator!=(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept;
constexpr bool operator<(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept;
constexpr bool operator>(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept;
constexpr bool operator<=(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept;
constexpr bool operator>=(const YearMonthDayLast &x, const YearMonthDayLast &y) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthDayLast operator+(const YearMonthDayLast &ymdl, const detail::Months &dm) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthDayLast operator+(const detail::Months &dm, const YearMonthDayLast &ymdl) noexcept;

constexpr YearMonthDayLast operator+(const YearMonthDayLast &ymdl, const detail::Years &dy) noexcept;

constexpr YearMonthDayLast operator+(const detail::Years &dy, const YearMonthDayLast &ymdl) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthDayLast operator-(const YearMonthDayLast &ymdl, const detail::Months &dm) noexcept;

constexpr YearMonthDayLast operator-(const YearMonthDayLast &ymdl, const detail::Years &dy) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthDayLast &ymdl);



RD_TIME_NAMESPACE_END

#include "year_month_day_last.inl"

#endif//RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_DAY_LAST_H_
