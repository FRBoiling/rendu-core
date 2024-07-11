/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_MONTH_MONTH_DAY_H_
#define RENDU_TIME_TIME_DATE_MONTH_MONTH_DAY_H_

#include "month.h"
#include "day.h"

RD_TIME_NAMESPACE_BEGIN

class MonthDay {
private:
  Month m_;
  Day d_;

public:
  MonthDay() = default;
  constexpr MonthDay(const Month &m, const Day &d) noexcept;

  constexpr Month month() const noexcept;
  constexpr Day day() const noexcept;

  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const MonthDay &x, const MonthDay &y) noexcept;
constexpr bool operator!=(const MonthDay &x, const MonthDay &y) noexcept;
constexpr bool operator<(const MonthDay &x, const MonthDay &y) noexcept;
constexpr bool operator>(const MonthDay &x, const MonthDay &y) noexcept;
constexpr bool operator<=(const MonthDay &x, const MonthDay &y) noexcept;
constexpr bool operator>=(const MonthDay &x, const MonthDay &y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const MonthDay &md);

constexpr const Day DaysInMonth365[] =
    {Day(31), Day(28), Day(31),
     Day(30), Day(31), Day(30),
     Day(31), Day(31), Day(30),
     Day(31), Day(30), Day(31)};

constexpr const Day DaysInMonth366[] =
    {Day(31), Day(29), Day(31),
     Day(30), Day(31), Day(30),
     Day(31), Day(31), Day(30),
     Day(31), Day(30), Day(31)};


const std::array<UInt, 13> s_daysToMonth365{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const std::array<UInt, 13> s_daysToMonth366{
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};


RD_TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_DATE_MONTH_MONTH_DAY_H_
