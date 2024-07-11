/*
* Created by boil on 2024/7/12.
*/

#include "date_operators.h"
#include "month_day.h"
#include "month_weekday.h"
#include "month_weekday_last.h"
#include "year_month.h"
#include "year_month_weekday.h"
#include "year_month_weekday_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline YearMonth operator/(const Year &y, const Month &m) noexcept {
  return {y, m};
}

constexpr inline YearMonth operator/(const Year &y, int m) noexcept {
  return y / Month(static_cast<unsigned>(m));
}


constexpr inline MonthDay operator/(const Month &m, const Day &d) noexcept {
  return {m, d};
}

constexpr inline MonthDay operator/(const Day &d, const Month &m) noexcept {
  return m / d;
}

constexpr inline MonthDay operator/(const Month &m, int d) noexcept {
  return m / Day(static_cast<unsigned>(d));
}

constexpr inline MonthDay operator/(int m, const Day &d) noexcept {
  return Month(static_cast<unsigned>(m)) / d;
}

constexpr inline MonthDay operator/(const Day &d, int m) noexcept {
  return m / d;
}


constexpr inline MonthDayLast operator/(const Month &m, LastSpec) noexcept {
  return MonthDayLast{m};
}

constexpr inline MonthDayLast operator/(LastSpec, const Month &m) noexcept {
  return m / last;
}

constexpr inline MonthDayLast operator/(int m, LastSpec) noexcept {
  return Month(static_cast<unsigned>(m)) / last;
}

constexpr inline MonthDayLast operator/(LastSpec, int m) noexcept {
  return m / last;
}


constexpr inline MonthWeekday operator/(const Month &m, const WeekdayIndexed &wdi) noexcept {
  return {m, wdi};
}

constexpr inline MonthWeekday operator/(const WeekdayIndexed &wdi, const Month &m) noexcept {
  return m / wdi;
}

constexpr inline MonthWeekday operator/(int m, const WeekdayIndexed &wdi) noexcept {
  return Month(static_cast<unsigned>(m)) / wdi;
}

constexpr inline MonthWeekday operator/(const WeekdayIndexed &wdi, int m) noexcept {
  return m / wdi;
}


constexpr inline MonthWeekdayLast operator/(const Month &m, const WeekdayLast &wdl) noexcept {
  return {m, wdl};
}

constexpr inline MonthWeekdayLast
operator/(const WeekdayLast &wdl, const Month &m) noexcept {
  return m / wdl;
}

constexpr inline MonthWeekdayLast operator/(int m, const WeekdayLast &wdl) noexcept {
  return Month(static_cast<unsigned>(m)) / wdl;
}

constexpr inline MonthWeekdayLast operator/(const WeekdayLast &wdl, int m) noexcept {
  return m / wdl;
}


constexpr inline YearMonthDay operator/(const YearMonth &ym, const Day &d) noexcept {
  return {ym.year(), ym.month(), d};
}

constexpr inline YearMonthDay operator/(const YearMonth &ym, int d) noexcept {
  return ym / Day(static_cast<unsigned>(d));
}

constexpr inline YearMonthDay operator/(const Year &y, const MonthDay &md) noexcept {
  return y / md.month() / md.day();
}

constexpr inline YearMonthDay operator/(int y, const MonthDay &md) noexcept {
  return Year(y) / md;
}

constexpr inline YearMonthDay operator/(const MonthDay &md, const Year &y) noexcept {
  return y / md;
}

constexpr inline YearMonthDay operator/(const MonthDay &md, int y) noexcept {
  return Year(y) / md;
}


constexpr inline YearMonthDayLast operator/(const YearMonth &ym, LastSpec) noexcept {
  return {ym.year(), MonthDayLast{ym.month()}};
}

constexpr inline YearMonthDayLast operator/(const Year &y, const MonthDayLast &mdl) noexcept {
  return {y, mdl};
}

constexpr inline YearMonthDayLast operator/(int y, const MonthDayLast &mdl) noexcept {
  return Year(y) / mdl;
}

constexpr inline YearMonthDayLast operator/(const MonthDayLast &mdl, const Year &y) noexcept {
  return y / mdl;
}

constexpr inline YearMonthDayLast operator/(const MonthDayLast &mdl, int y) noexcept {
  return Year(y) / mdl;
}


constexpr inline YearMonthWeekday operator/(const YearMonth &ym, const WeekdayIndexed &wdi) noexcept {
  return {ym.year(), ym.month(), wdi};
}

constexpr inline YearMonthWeekday operator/(const Year &y, const MonthWeekday &mwd) noexcept {
  return {y, mwd.month(), mwd.weekday_indexed()};
}

constexpr inline YearMonthWeekday operator/(int y, const MonthWeekday &mwd) noexcept {
  return Year(y) / mwd;
}

constexpr inline YearMonthWeekday operator/(const MonthWeekday &mwd, const Year &y) noexcept {
  return y / mwd;
}

constexpr inline YearMonthWeekday operator/(const MonthWeekday &mwd, int y) noexcept {
  return Year(y) / mwd;
}


constexpr inline YearMonthWeekdayLast
operator/(const YearMonth &ym, const WeekdayLast &wdl) noexcept {
  return {ym.year(), ym.month(), wdl};
}

constexpr inline YearMonthWeekdayLast operator/(const Year &y, const MonthWeekdayLast &mwdl) noexcept {
  return {y, mwdl.month(), mwdl.weekday_last()};
}

constexpr inline YearMonthWeekdayLast operator/(int y, const MonthWeekdayLast &mwdl) noexcept {
  return Year(y) / mwdl;
}

constexpr inline YearMonthWeekdayLast operator/(const MonthWeekdayLast &mwdl, const Year &y) noexcept {
  return y / mwdl;
}

constexpr inline YearMonthWeekdayLast operator/(const MonthWeekdayLast &mwdl, int y) noexcept {
  return Year(y) / mwdl;
}

RD_TIME_NAMESPACE_END
