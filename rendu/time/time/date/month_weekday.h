/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_MONTH_MONTH_WEEKDAY_H_
#define RENDU_TIME_TIME_DATE_MONTH_MONTH_WEEKDAY_H_

#include "month.h"
#include "weekday_indexed.h"

RD_TIME_NAMESPACE_BEGIN

class MonthWeekday {

private:
  Month m_;
  WeekdayIndexed wdi_;

public:
  constexpr MonthWeekday(const Month &m, const WeekdayIndexed &wdi) noexcept;

  constexpr Month month() const noexcept;
  constexpr WeekdayIndexed weekday_indexed() const noexcept;

  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const MonthWeekday &x, const MonthWeekday &y) noexcept;
constexpr bool operator!=(const MonthWeekday &x, const MonthWeekday &y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthWeekday &mwd);



RD_TIME_NAMESPACE_END


#endif//RENDU_TIME_TIME_DATE_MONTH_MONTH_WEEKDAY_H_
