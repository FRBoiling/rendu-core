/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_WEEKDAY_LAST_H_
#define RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_WEEKDAY_LAST_H_

#include "year.h"
#include "month.h"
#include "weekday_last.h"

RD_TIME_NAMESPACE_BEGIN

class YearMonthWeekdayLast {
private:
  Year y_;
  Month m_;
  WeekdayLast wdl_;

public:
  constexpr YearMonthWeekdayLast(const Year &y, const Month &m, const WeekdayLast &wdl) noexcept;

  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthWeekdayLast &operator+=(const detail::Months &m) noexcept;
  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthWeekdayLast &operator-=(const detail::Months &m) noexcept;
  constexpr YearMonthWeekdayLast &operator+=(const detail::Years &y) noexcept;
  constexpr YearMonthWeekdayLast &operator-=(const detail::Years &y) noexcept;

  constexpr Year year() const noexcept;
  constexpr Month month() const noexcept;
  constexpr Weekday weekday() const noexcept;
  constexpr WeekdayLast weekday_last() const noexcept;

  constexpr operator detail::SysDays() const noexcept;
  constexpr explicit operator detail::LocalDays() const noexcept;
  constexpr bool ok() const noexcept;

private:
  constexpr detail::Days to_days() const noexcept;
};

constexpr bool operator==(const YearMonthWeekdayLast &x, const YearMonthWeekdayLast &y) noexcept;

constexpr bool operator!=(const YearMonthWeekdayLast &x, const YearMonthWeekdayLast &y) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthWeekdayLast operator+(const YearMonthWeekdayLast &ymwdl, const detail::Months &dm) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthWeekdayLast operator+(const detail::Months &dm, const YearMonthWeekdayLast &ymwdl) noexcept;

constexpr YearMonthWeekdayLast operator+(const YearMonthWeekdayLast &ymwdl, const detail::Years &dy) noexcept;

constexpr YearMonthWeekdayLast operator+(const detail::Years &dy, const YearMonthWeekdayLast &ymwdl) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthWeekdayLast operator-(const YearMonthWeekdayLast &ymwdl, const detail::Months &dm) noexcept;

constexpr YearMonthWeekdayLast operator-(const YearMonthWeekdayLast &ymwdl, const detail::Years &dy) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthWeekdayLast &ymwdl);

RD_TIME_NAMESPACE_END

#include "year_month_weekday_last.inl"

#endif//RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_WEEKDAY_LAST_H_
