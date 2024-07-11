/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_LAST_H_
#define RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_LAST_H_

#include "weekday.h"

RD_TIME_NAMESPACE_BEGIN

class WeekdayLast
{
private:
  Weekday wd_;
  
public:
  explicit constexpr WeekdayLast(const Weekday& wd) noexcept;

  constexpr Weekday weekday() const noexcept;
  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const WeekdayLast& x, const WeekdayLast& y) noexcept;
constexpr bool operator!=(const WeekdayLast& x, const WeekdayLast& y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const WeekdayLast& wdl);

RD_TIME_NAMESPACE_END

#include "weekday_last.inl"

#endif//RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_LAST_H_
