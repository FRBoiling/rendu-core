/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_MONTH_MONTH_DAY_LAST_H_
#define RENDU_TIME_TIME_DATE_MONTH_MONTH_DAY_LAST_H_

#include "month.h"

RD_TIME_NAMESPACE_BEGIN

class MonthDayLast
{
private:
  Month m_;
  
public:
  constexpr explicit MonthDayLast(const Month& m) noexcept;

  constexpr Month month() const noexcept;
  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const MonthDayLast& x, const MonthDayLast& y) noexcept;
constexpr bool operator!=(const MonthDayLast& x, const MonthDayLast& y) noexcept;
constexpr bool operator< (const MonthDayLast& x, const MonthDayLast& y) noexcept;
constexpr bool operator> (const MonthDayLast& x, const MonthDayLast& y) noexcept;
constexpr bool operator<=(const MonthDayLast& x, const MonthDayLast& y) noexcept;
constexpr bool operator>=(const MonthDayLast& x, const MonthDayLast& y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const MonthDayLast& mdl);



RD_TIME_NAMESPACE_END

#include "month_day_last.inl"

#endif//RENDU_TIME_TIME_DATE_MONTH_MONTH_DAY_LAST_H_
