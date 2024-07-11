/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_WEEKDAY_H_
#define RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_WEEKDAY_H_

#include "year.h"
#include "month.h"
#include "weekday_indexed.h"

RD_TIME_NAMESPACE_BEGIN

class YearMonthWeekday {
private:
  Year y_;
  Month m_;
  WeekdayIndexed wdi_;

public:
  YearMonthWeekday() = default;
  constexpr YearMonthWeekday(const Year &y, const Month &m,
                             const WeekdayIndexed &wdi) noexcept;
  constexpr YearMonthWeekday(const detail::SysDays &dp) noexcept;
  constexpr explicit YearMonthWeekday(const detail::LocalDays &dp) noexcept;

  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthWeekday &operator+=(const detail::Months &m) noexcept;
  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonthWeekday &operator-=(const detail::Months &m) noexcept;
  constexpr YearMonthWeekday &operator+=(const detail::Years &y) noexcept;
  constexpr YearMonthWeekday &operator-=(const detail::Years &y) noexcept;

  constexpr Year year() const noexcept;
  constexpr Month month() const noexcept;
  constexpr Weekday weekday() const noexcept;
  constexpr unsigned index() const noexcept;
  constexpr WeekdayIndexed weekday_indexed() const noexcept;

  constexpr operator detail::SysDays() const noexcept;
  constexpr explicit operator detail::LocalDays() const noexcept;
  constexpr bool ok() const noexcept;

private:
  static constexpr YearMonthWeekday from_days(detail::Days dp) noexcept;
  constexpr detail::Days to_days() const noexcept;
};

constexpr bool operator==(const YearMonthWeekday &x, const YearMonthWeekday &y) noexcept;
constexpr bool operator!=(const YearMonthWeekday &x, const YearMonthWeekday &y) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthWeekday operator+(const YearMonthWeekday &ymwd, const detail::Months &dm) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthWeekday operator+(const detail::Months &dm, const YearMonthWeekday &ymwd) noexcept;

constexpr YearMonthWeekday operator+(const YearMonthWeekday &ymwd, const detail::Years &dy) noexcept;

constexpr YearMonthWeekday operator+(const detail::Years &dy, const YearMonthWeekday &ymwd) noexcept;


template<class = detail::unspecified_month_disambiguator>
constexpr YearMonthWeekday operator-(const YearMonthWeekday &ymwd, const detail::Months &dm) noexcept;

constexpr YearMonthWeekday operator-(const YearMonthWeekday &ymwd, const detail::Years &dy) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthWeekday &ymwdi);


RD_TIME_NAMESPACE_END

#include "year_month_weekday.inl"

#endif//RENDU_TIME_TIME_DATE_YEAR_YEAR_MONTH_WEEKDAY_H_
