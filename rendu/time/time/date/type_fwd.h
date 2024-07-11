/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_FWD_H_
#define RENDU_TIME_TIME_DATE_FWD_H_

#include "time_point.h"

RD_TIME_NAMESPACE_BEGIN

struct LastSpec {
  explicit LastSpec() = default;
};

class Day;
class Month;
class Year;

class Weekday;
class WeekdayIndexed;
class WeekdayLast;

class MonthDay;
class MonthDayLast;

class MonthWeekday;
class MonthWeekdayLast;

class YearMonth;

class YearMonthDay;
class YearMonthDayLast;
class YearMonthWeekday;
class YearMonthWeekdayLast;

constexpr const LastSpec last{};

RD_TIME_NAMESPACE_END

#endif//RENDU_TIME_TIME_DATE_FWD_H_
