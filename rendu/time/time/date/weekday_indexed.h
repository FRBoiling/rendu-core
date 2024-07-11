/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_INDEXED_H_
#define RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_INDEXED_H_

#include "weekday.h"

RD_TIME_NAMESPACE_BEGIN

class WeekdayIndexed
{
private:
  unsigned char wd_    : 4;
  unsigned char index_ : 4;
  
public:
  WeekdayIndexed() = default;
  constexpr WeekdayIndexed(const Weekday& wd, unsigned index) noexcept;

  constexpr Weekday weekday() const noexcept;
  constexpr unsigned index() const noexcept;
  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const WeekdayIndexed& x, const WeekdayIndexed& y) noexcept;
constexpr bool operator!=(const WeekdayIndexed& x, const WeekdayIndexed& y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const WeekdayIndexed& wdi);

RD_TIME_NAMESPACE_END

#include "weekday_indexed.inl"

#endif//RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_INDEXED_H_
