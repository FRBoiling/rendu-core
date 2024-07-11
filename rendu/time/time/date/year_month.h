/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_MONTH_YEAR_MONTH_H_
#define RENDU_TIME_TIME_DATE_MONTH_YEAR_MONTH_H_

#include "year.h"
#include "month.h"

RD_TIME_NAMESPACE_BEGIN

class YearMonth {
private:
  Year y_;
  Month m_;

public:
  YearMonth() = default;
  constexpr YearMonth(const Year &y, const Month &m) noexcept;

  constexpr Year year() const noexcept;
  constexpr Month month() const noexcept;

  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonth &operator+=(const detail::Months &dm) noexcept;
  template<class = detail::unspecified_month_disambiguator>
  constexpr YearMonth &operator-=(const detail::Months &dm) noexcept;
  constexpr YearMonth &operator+=(const detail::Years &dy) noexcept;
  constexpr YearMonth &operator-=(const detail::Years &dy) noexcept;

  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const YearMonth &x, const YearMonth &y) noexcept;
constexpr bool operator!=(const YearMonth &x, const YearMonth &y) noexcept;
constexpr bool operator<(const YearMonth &x, const YearMonth &y) noexcept;
constexpr bool operator>(const YearMonth &x, const YearMonth &y) noexcept;
constexpr bool operator<=(const YearMonth &x, const YearMonth &y) noexcept;
constexpr bool operator>=(const YearMonth &x, const YearMonth &y) noexcept;

template<class = detail::unspecified_month_disambiguator>
constexpr YearMonth operator+(const YearMonth &ym, const detail::Months &dm) noexcept;
template<class = detail::unspecified_month_disambiguator>
constexpr YearMonth operator+(const detail::Months &dm, const YearMonth &ym) noexcept;
template<class = detail::unspecified_month_disambiguator>
constexpr YearMonth operator-(const YearMonth &ym, const detail::Months &dm) noexcept;

constexpr detail::Months operator-(const YearMonth &x, const YearMonth &y) noexcept;
constexpr YearMonth operator+(const YearMonth &ym, const detail::Years &dy) noexcept;
constexpr YearMonth operator+(const detail::Years &dy, const YearMonth &ym) noexcept;
constexpr YearMonth operator-(const YearMonth &ym, const detail::Years &dy) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonth &ym);


RD_TIME_NAMESPACE_END

#include "year_month.inl"

#endif//RENDU_TIME_TIME_DATE_MONTH_YEAR_MONTH_H_
