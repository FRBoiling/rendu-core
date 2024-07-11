/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_DAY_H_
#define RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_DAY_H_

#include "year.h"
#include "month.h"
#include "day.h"

RD_TIME_NAMESPACE_BEGIN

class YearMonthDay {
private:
  Year y_;
  Month m_;
  Day d_;

public:
  YearMonthDay() = default;
  constexpr YearMonthDay(const Year &y, const Month &m, const Day &d) noexcept;
  constexpr YearMonthDay(const YearMonthDayLast &ymdl) noexcept;

  constexpr YearMonthDay(detail::SysDays dp) noexcept;
  constexpr explicit YearMonthDay(detail::LocalDays dp) noexcept;

  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthDay &operator+=(const detail::Months &m) noexcept;
  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthDay &operator-=(const detail::Months &m) noexcept;

  constexpr YearMonthDay &operator+=(const detail::Years &y) noexcept;
  constexpr YearMonthDay &operator-=(const detail::Years &y) noexcept;

  constexpr Year year() const noexcept;
  constexpr Month month() const noexcept;
  constexpr Day day() const noexcept;

  constexpr operator detail::SysDays() const noexcept;
  constexpr explicit operator detail::LocalDays() const noexcept;
  constexpr bool ok() const noexcept;

private:
  static constexpr YearMonthDay from_days(detail::Days dp) noexcept;
  constexpr detail::Days to_days() const noexcept;
};

constexpr bool operator==(const YearMonthDay &x, const YearMonthDay &y) noexcept;
constexpr bool operator!=(const YearMonthDay &x, const YearMonthDay &y) noexcept;
constexpr bool operator<(const YearMonthDay &x, const YearMonthDay &y) noexcept;
constexpr bool operator>(const YearMonthDay &x, const YearMonthDay &y) noexcept;
constexpr bool operator<=(const YearMonthDay &x, const YearMonthDay &y) noexcept;
constexpr bool operator>=(const YearMonthDay &x, const YearMonthDay &y) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthDay operator+(const YearMonthDay &ymd, const detail::Months &dm) noexcept;
template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthDay operator+(const detail::Months &dm, const YearMonthDay &ymd) noexcept;
template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthDay operator-(const YearMonthDay &ymd, const detail::Months &dm) noexcept;
constexpr YearMonthDay operator+(const YearMonthDay &ymd, const detail::Years &dy) noexcept;
constexpr YearMonthDay operator+(const detail::Years &dy, const YearMonthDay &ymd) noexcept;
constexpr YearMonthDay operator-(const YearMonthDay &ymd, const detail::Years &dy) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthDay &ymd);



RD_TIME_NAMESPACE_END

#include "year_month_day.inl"

#endif//RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_DAY_H_
