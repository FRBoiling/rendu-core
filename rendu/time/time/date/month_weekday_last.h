/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_MONTH_MONTH_WEEKDAY_LAST_H_
#define RENDU_TIME_TIME_DATE_MONTH_MONTH_WEEKDAY_LAST_H_

#include "month.h"
#include "weekday_last.h"

RD_TIME_NAMESPACE_BEGIN

class MonthWeekdayLast {
private:
  Month m_;
  WeekdayLast wdl_;

public:
  constexpr MonthWeekdayLast(const Month &m, const WeekdayLast &wd) noexcept;

  constexpr Month month() const noexcept;
  constexpr WeekdayLast weekday_last() const noexcept;

  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const MonthWeekdayLast &x, const MonthWeekdayLast &y) noexcept;
constexpr bool operator!=(const MonthWeekdayLast &x, const MonthWeekdayLast &y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthWeekdayLast &mwdl);


RD_TIME_NAMESPACE_END

#include "month_weekday_last.inl"

#endif//RENDU_TIME_TIME_DATE_MONTH_MONTH_WEEKDAY_LAST_H_
