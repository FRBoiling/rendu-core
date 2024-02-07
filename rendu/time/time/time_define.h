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
using namespace std::chrono_literals;

TIME_NAMESPACE_BEGIN

const int MicrosecondsPerMillisecond = 1000;
const Long TicksPerMicrosecond = 10;
const Long TicksPerMillisecond = TicksPerMicrosecond * MicrosecondsPerMillisecond;

const int HoursPerDay = 24;
const int MinutesPerHour = 60;
const int SecsPerMinute = 60;
const int MillisPerSecond = 1000;

const Long TicksPerSecond = TicksPerMillisecond * 1000;
const Long TicksPerMinute = TicksPerSecond * 60;
const Long TicksPerHour = TicksPerMinute * 60;
const Long TicksPerDay = TicksPerHour * HoursPerDay;

// Number of milliseconds per time unit
const int MillisPerMinute = MillisPerSecond * 60;
const int MillisPerHour = MillisPerMinute * 60;
const int MillisPerDay = MillisPerHour * HoursPerDay;

// Number of days in a non-leap year
const int DaysPerYear = 365;
// Number of days in 4 years
const int DaysPer4Years = DaysPerYear * 4 + 1;      // 1461
                                                      // Number of days in 100 years
const int DaysPer100Years = DaysPer4Years * 25 - 1; // 36524
                                                      // Number of days in 400 years
const int DaysPer400Years = DaysPer100Years * 4 + 1;// 146097

// Number of days from 1/1/0001 to 12/31/1600
const int DaysTo1601 = DaysPer400Years * 4;// 584388
                                             // Number of days from 1/1/0001 to 12/30/1899
const int DaysTo1899 = DaysPer400Years * 4 + DaysPer100Years * 3 - 367;
// Number of days from 1/1/0001 to 12/31/1969
const int DaysTo1970 = DaysPer400Years * 4 + DaysPer100Years * 3 + DaysPer4Years * 17 + DaysPerYear;// 719,162
                                                                                                      // Number of days from 1/1/0001 to 12/31/9999
const int DaysTo10000 = DaysPer400Years * 25 - 366;                                                 // 3652059

const Long MinTicks = 0;
const Long MaxTicks = DaysTo10000 * TicksPerDay - 1;
const Long MaxMicroseconds = MaxTicks / TicksPerMicrosecond;
const Long MaxMillis = MaxTicks / TicksPerMillisecond;
const Long MaxSeconds = MaxTicks / TicksPerSecond;
const Long MaxMinutes = MaxTicks / TicksPerMinute;
const Long MaxHours = MaxTicks / TicksPerHour;
const Long MaxDays = (Long) DaysTo10000 - 1;

const Long UnixEpochTicks = DaysTo1970 * TicksPerDay;
const Long FileTimeOffset = DaysTo1601 * TicksPerDay;
const Long DoubleDateOffset = DaysTo1899 * TicksPerDay;
// The minimum OA date is 0100/01/01 (Note it's year 100).
// The maximum OA date is 9999/12/31
const Long OADateMinAsTicks = (DaysPer100Years - DaysPerYear) * TicksPerDay;
// All OA dates must be greater than (not >=) OADateMinAsDouble
const double OADateMinAsDouble = -657435.0;
// All OA dates must be less than (not <=) OADateMaxAsDouble
const double OADateMaxAsDouble = 2958466.0;

// Euclidean Affine Functions Algorithm (EAF) constants

// Constants used for fast calculation of following subexpressions
//      x / DaysPer4Years
//      x % DaysPer4Years / 4
const uint EafMultiplier = (uint) (((1UL << 32) + DaysPer4Years - 1) / DaysPer4Years);// 2,939,745
const uint EafDivider = EafMultiplier * 4;                                              // 11,758,980

const uLong TicksPer6Hours = TicksPerHour * 6;
const int March1BasedDayOfNewYear = 306;// Days between March 1 and January 1

const std::array<uint, 13> s_daysToMonth365{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const std::array<uint, 13> s_daysToMonth366{
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

const std::array<char, 12> DaysInMonth365{
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const std::array<char, 12> DaysInMonth366{
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const uLong TicksMask = 0x3FFFFFFFFFFFFFFF;
const uLong FlagsMask = 0xC000000000000000;
const Long TicksCeiling = 0x4000000000000000;
const uLong KindUnspecified = 0x0000000000000000;
const uLong KindUtc = 0x4000000000000000;
const uLong KindLocal = 0x8000000000000000;
const uLong KindLocalAmbiguousDst = 0xC000000000000000;
const int KindShift = 62;

const string TicksField = "ticks";      // Do not rename (binary serialization)
const string DateDataField = "dateData";// Do not rename (binary serialization)

TIME_NAMESPACE_END

#endif//RENDU_TIME_DEFINE_H
