/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_TIME_DEFINE_H
#define RENDU_TIME_DEFINE_H

#include "base/base.hpp"
#include <chrono>

#define TIME_NAMESPACE_BEGIN \
  namespace rendu {          \
    namespace time {

#define TIME_NAMESPACE_END \
  } /*namespace time*/     \
  } /*namespace rendu*/


using namespace rendu;

TIME_NAMESPACE_BEGIN

const INT32 MicrosecondsPerMillisecond = 1000;
const INT64 TicksPerMicrosecond = 10;
const INT64 TicksPerMillisecond = TicksPerMicrosecond * MicrosecondsPerMillisecond;

const INT32 HoursPerDay = 24;
const INT32 MinutesPerHour = 60;
const INT32 SecsPerMinute = 60;
const INT32 MillisPerSecond = 1000;

const INT64 TicksPerSecond = TicksPerMillisecond * 1000;
const INT64 TicksPerMinute = TicksPerSecond * 60;
const INT64 TicksPerHour = TicksPerMinute * 60;
const INT64 TicksPerDay = TicksPerHour * HoursPerDay;

// Number of milliseconds per time unit
const INT32 MillisPerMinute = MillisPerSecond * 60;
const INT32 MillisPerHour = MillisPerMinute * 60;
const INT32 MillisPerDay = MillisPerHour * HoursPerDay;

// Number of days in a non-leap year
const INT32 DaysPerYear = 365;
// Number of days in 4 years
const INT32 DaysPer4Years = DaysPerYear * 4 + 1;      // 1461
                                                      // Number of days in 100 years
const INT32 DaysPer100Years = DaysPer4Years * 25 - 1; // 36524
                                                      // Number of days in 400 years
const INT32 DaysPer400Years = DaysPer100Years * 4 + 1;// 146097

// Number of days from 1/1/0001 to 12/31/1600
const INT32 DaysTo1601 = DaysPer400Years * 4;// 584388
                                             // Number of days from 1/1/0001 to 12/30/1899
const INT32 DaysTo1899 = DaysPer400Years * 4 + DaysPer100Years * 3 - 367;
// Number of days from 1/1/0001 to 12/31/1969
const INT32 DaysTo1970 = DaysPer400Years * 4 + DaysPer100Years * 3 + DaysPer4Years * 17 + DaysPerYear;// 719,162
                                                                                                      // Number of days from 1/1/0001 to 12/31/9999
const INT32 DaysTo10000 = DaysPer400Years * 25 - 366;                                                 // 3652059

const INT64 MinTicks = 0;
const INT64 MaxTicks = DaysTo10000 * TicksPerDay - 1;
const INT64 MaxMicroseconds = MaxTicks / TicksPerMicrosecond;
const INT64 MaxMillis = MaxTicks / TicksPerMillisecond;
const INT64 MaxSeconds = MaxTicks / TicksPerSecond;
const INT64 MaxMinutes = MaxTicks / TicksPerMinute;
const INT64 MaxHours = MaxTicks / TicksPerHour;
const INT64 MaxDays = (INT64) DaysTo10000 - 1;

const INT64 UnixEpochTicks = DaysTo1970 * TicksPerDay;
const INT64 FileTimeOffset = DaysTo1601 * TicksPerDay;
const INT64 DoubleDateOffset = DaysTo1899 * TicksPerDay;
// The minimum OA date is 0100/01/01 (Note it's year 100).
// The maximum OA date is 9999/12/31
const INT64 OADateMinAsTicks = (DaysPer100Years - DaysPerYear) * TicksPerDay;
// All OA dates must be greater than (not >=) OADateMinAsDouble
const double OADateMinAsDouble = -657435.0;
// All OA dates must be less than (not <=) OADateMaxAsDouble
const double OADateMaxAsDouble = 2958466.0;

// Euclidean Affine Functions Algorithm (EAF) constants

// Constants used for fast calculation of following subexpressions
//      x / DaysPer4Years
//      x % DaysPer4Years / 4
const UINT32 EafMultiplier = (UINT32) (((1UL << 32) + DaysPer4Years - 1) / DaysPer4Years);// 2,939,745
const UINT32 EafDivider = EafMultiplier * 4;                                              // 11,758,980

const UINT64 TicksPer6Hours = TicksPerHour * 6;
const INT32 March1BasedDayOfNewYear = 306;// Days between March 1 and January 1

const std::array<UINT32, 13> s_daysToMonth365{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const std::array<UINT32, 13> s_daysToMonth366{
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

const std::array<INT8, 12> DaysInMonth365{
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const std::array<INT8, 12> DaysInMonth366{
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const UINT64 TicksMask = 0x3FFFFFFFFFFFFFFF;
const UINT64 FlagsMask = 0xC000000000000000;
const INT64 TicksCeiling = 0x4000000000000000;
const UINT64 KindUnspecified = 0x0000000000000000;
const UINT64 KindUtc = 0x4000000000000000;
const UINT64 KindLocal = 0x8000000000000000;
const UINT64 KindLocalAmbiguousDst = 0xC000000000000000;
const INT32 KindShift = 62;

const STRING TicksField = "ticks";      // Do not rename (binary serialization)
const STRING DateDataField = "dateData";// Do not rename (binary serialization)

TIME_NAMESPACE_END
#endif//RENDU_TIME_DEFINE_H
