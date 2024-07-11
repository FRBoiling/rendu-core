/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_DATE_OPERATORS_H_
#define RENDU_TIME_TIME_DATE_DATE_OPERATORS_H_

#include "type_fwd.h"

RD_TIME_NAMESPACE_BEGIN

constexpr YearMonth operator/(const Year &y, const Month &m) noexcept;
constexpr YearMonth operator/(const Year &y, int m) noexcept;

constexpr MonthDay operator/(const Day &d, const Month &m) noexcept;
constexpr MonthDay operator/(const Day &d, int m) noexcept;
constexpr MonthDay operator/(const Month &m, const Day &d) noexcept;
constexpr MonthDay operator/(const Month &m, int d) noexcept;
constexpr MonthDay operator/(int m, const Day &d) noexcept;


constexpr MonthDayLast operator/(const Month &m, LastSpec) noexcept;
constexpr MonthDayLast operator/(int m, LastSpec) noexcept;
constexpr MonthDayLast operator/(LastSpec, const Month &m) noexcept;
constexpr MonthDayLast operator/(LastSpec, int m) noexcept;


constexpr MonthWeekday operator/(const Month &m, const WeekdayIndexed &wdi) noexcept;
constexpr MonthWeekday operator/(int m, const WeekdayIndexed &wdi) noexcept;
constexpr MonthWeekday operator/(const WeekdayIndexed &wdi, const Month &m) noexcept;
constexpr MonthWeekday operator/(const WeekdayIndexed &wdi, int m) noexcept;


constexpr MonthWeekdayLast operator/(const Month &m, const WeekdayLast &wdl) noexcept;
constexpr MonthWeekdayLast operator/(int m, const WeekdayLast &wdl) noexcept;
constexpr MonthWeekdayLast operator/(const WeekdayLast &wdl, const Month &m) noexcept;
constexpr MonthWeekdayLast operator/(const WeekdayLast &wdl, int m) noexcept;


constexpr YearMonthDay operator/(const YearMonth &ym, const Day &d) noexcept;
constexpr YearMonthDay operator/(const YearMonth &ym, int d) noexcept;
constexpr YearMonthDay operator/(const Year &y, const MonthDay &md) noexcept;
constexpr YearMonthDay operator/(int y, const MonthDay &md) noexcept;
constexpr YearMonthDay operator/(const MonthDay &md, const Year &y) noexcept;
constexpr YearMonthDay operator/(const MonthDay &md, int y) noexcept;


constexpr YearMonthDayLast operator/(const YearMonth &ym, LastSpec) noexcept;
constexpr YearMonthDayLast operator/(const Year &y, const MonthDayLast &mdl) noexcept;
constexpr YearMonthDayLast operator/(int y, const MonthDayLast &mdl) noexcept;
constexpr YearMonthDayLast operator/(const MonthDayLast &mdl, const Year &y) noexcept;
constexpr YearMonthDayLast operator/(const MonthDayLast &mdl, int y) noexcept;


constexpr YearMonthWeekday operator/(const YearMonth &ym, const WeekdayIndexed &wdi) noexcept;
constexpr YearMonthWeekday operator/(const Year &y, const MonthWeekday &mwd) noexcept;
constexpr YearMonthWeekday operator/(int y, const MonthWeekday &mwd) noexcept;
constexpr YearMonthWeekday operator/(const MonthWeekday &mwd, const Year &y) noexcept;
constexpr YearMonthWeekday operator/(const MonthWeekday &mwd, int y) noexcept;

constexpr YearMonthWeekdayLast operator/(const YearMonth &ym, const WeekdayLast &wdl) noexcept;
constexpr YearMonthWeekdayLast operator/(const Year &y, const MonthWeekdayLast &mwdl) noexcept;
constexpr YearMonthWeekdayLast operator/(int y, const MonthWeekdayLast &mwdl) noexcept;
constexpr YearMonthWeekdayLast operator/(const MonthWeekdayLast &mwdl, const Year &y) noexcept;
constexpr YearMonthWeekdayLast operator/(const MonthWeekdayLast &mwdl, int y) noexcept;

RD_TIME_NAMESPACE_END

#include "date_operators.inl"

#endif//RENDU_TIME_TIME_DATE_DATE_OPERATORS_H_
