/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_INCLUDE_DURATIONS_H_
#define RENDU_TIME_TIME_DATE_INCLUDE_DURATIONS_H_

#include "time_define.h"
RD_TIME_NAMESPACE_BEGIN

// Work around for a NVCC compiler bug which causes it to fail
// to compile std::ratio_{multiply,divide} when used directly
// in the std::chrono::duration template instantiations below
RD_DETAIL_NAMESPACE_BEGIN
template<typename R1, typename R2>
using ratio_multiply = decltype(std::ratio_multiply<R1, R2>{});

template<typename R1, typename R2>
using ratio_divide = decltype(std::ratio_divide<R1, R2>{});

const Int MicrosecondsPerMillisecond = 1000;
const Int MillisPerSecond = 1000;
const Int SecsPerMinute = 60;
const Int MinutesPerHour = 60;
const Int HoursPerDay = 24;
const Int DaysPerWeek = 7;
const Int MonthsPerYear = 12;

const Long TicksPerMicrosecond = 10;

const Long TicksPerMillisecond = TicksPerMicrosecond * MicrosecondsPerMillisecond;
const Long TicksPerSecond = TicksPerMillisecond * MillisPerSecond;
const Long TicksPerMinute = TicksPerSecond * SecsPerMinute;
const Long TicksPerHour = TicksPerMinute * MinutesPerHour;
const Long TicksPerDay = TicksPerHour * HoursPerDay;


const Long MicrosecondsPerSecond = MicrosecondsPerMillisecond * MillisPerSecond;
const Long MicrosecondsPerMinute = MicrosecondsPerSecond * SecsPerMinute;
const Long MicrosecondsPerHour = MicrosecondsPerMinute * MinutesPerHour;
const Long MicrosecondsPerDay = MicrosecondsPerHour * HoursPerDay;

// Number of milliseconds per time unit
const Int MillisPerMinute = MillisPerSecond * SecsPerMinute;
const Int MillisPerHour = MillisPerMinute * MinutesPerHour;
const Int MillisPerDay = MillisPerHour * HoursPerDay;


const Int SecsPerHour = SecsPerMinute * MinutesPerHour;


// Number of days in a non-leap year
const Int DaysPerYear = 365;
// Number of days in 4 years
const Int DaysPer4Years = DaysPerYear * 4 + 1;      // 1461
                                                    // Number of days in 100 years
const Int DaysPer100Years = DaysPer4Years * 25 - 1; // 36524
                                                    // Number of days in 400 years
const Int DaysPer400Years = DaysPer100Years * 4 + 1;// 146097


// Number of days from 1/1/0001 to 12/31/1600
const Int DaysTo1601 = DaysPer400Years * 4;// 584388
                                           // Number of days from 1/1/0001 to 12/30/1899
const Int DaysTo1899 = DaysPer400Years * 4 + DaysPer100Years * 3 - 367;
// Number of days from 1/1/0001 to 12/31/1969
const Int DaysTo1970 = DaysPer400Years * 4 + DaysPer100Years * 3 + DaysPer4Years * 17 + DaysPerYear;// 719,162
                                                                                                    // Number of days from 1/1/0001 to 12/31/9999
const Int DaysTo10000 = DaysPer400Years * 25 - 366;                                                 // 3652059


const Int March1BasedDayOfNewYear = 306;// Days between March 1 and January 1


const Long MinTicks = 0;
const Long MaxTicks = DaysTo10000 * TicksPerDay - 1;
const Long MaxMicroseconds = MaxTicks / TicksPerMicrosecond;
const Long MaxMillis = MaxTicks / TicksPerMillisecond;
const Long MaxSeconds = MaxTicks / TicksPerSecond;
const Long MaxMinutes = MaxTicks / TicksPerMinute;
const Long MaxHours = MaxTicks / TicksPerHour;
const Long MaxDays = (Long) DaysTo10000 - 1;

const Long UnixEpochTicks = DaysTo1970 * TicksPerDay;
const Long UnixEpochMicros = DaysTo1970 * MicrosecondsPerDay;
const Long FileTimeOffset = DaysTo1601 * TicksPerDay;
const Long DoubleDateOffset = DaysTo1899 * TicksPerDay;
// The minimum OA date is 0100/01/01 (Note it's year 100).
// The maximum OA date is 9999/12/31
const Long OADateMinAsTicks = (DaysPer100Years - DaysPerYear) * TicksPerDay;
// All OA dates must be greater than (not >=) OADateMinAsDouble
const Double OADateMinAsDouble = -657435.0;
// All OA dates must be less than (not <=) OADateMaxAsDouble
const Double OADateMaxAsDouble = 2958466.0;

const ULong TicksPer6Hours = TicksPerHour * 6;


// Euclidean Affine Functions Algorithm (EAF) constants

// Constants used for fast calculation of following subexpressions
//      x / DaysPer4Years
//      x % DaysPer4Years / 4
const UInt EafMultiplier = (UInt) (((1UL << 32) + DaysPer4Years - 1) / DaysPer4Years);// 2,939,745
const UInt EafDivider = EafMultiplier * 4;                                            // 11,758,980

const ULong TicksMask = 0x3FFFFFFFFFFFFFFF;
const ULong FlagsMask = 0xC000000000000000;
const Long TicksCeiling = 0x4000000000000000;
const ULong KindUnspecified = 0x0000000000000000;
const ULong KindUtc = 0x4000000000000000;
const ULong KindLocal = 0x8000000000000000;
const ULong KindLocalAmbiguousDst = 0xC000000000000000;
const Int KindShift = 62;


using Nanoseconds = std::chrono::nanoseconds;  //纳秒
using Microseconds = std::chrono::microseconds;//微妙
using Milliseconds = std::chrono::milliseconds;//毫秒
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::duration<Long, std::ratio<SecsPerMinute>>;
using Hours = std::chrono::duration<Long, std::ratio<SecsPerHour>>;

using Days = std::chrono::duration<Int, detail::ratio_multiply<std::ratio<HoursPerDay>, Hours::period>>;
using Weeks = std::chrono::duration<Int, detail::ratio_multiply<std::ratio<DaysPerWeek>, Days::period>>;

//年的概念，是地球围绕太阳一周的时间（所谓公转周期）称为一年，这个周期是相当稳定的，很长时间也不会变动1秒，
//但是，很遗憾的是这个周期并不能被一天的时间整除，真正的一年其实是365.2423天（目前）。
//所以，如果每年定义为365天的话，1年就会多出0.2423天，4年就会多出0.9692天，非常接近1天，这样闰年就出现了，也就是每4年要有1年设置为366天，来抵消这多出的1天。
// 1）如果年份能被4整除，则该年计为闰年；可是，假如不做调整的话，当400年的时候，累计才多出96.92天，闰年的设置却多出来100天，所以要在400年内，再撤销3个闰年。怎么撤销呢？就有了下面这个规则：
// 2）如果年份能被100整除，则不计为闰年；问题又来了，400年里有4个，又少了一个，所以再加一个规则就是：
// 3）如果年份能被400整除，则计为闰年。这样每400年里设置了97个闰年，误差被调整到400年仅有0.08天，而设置规则也并不是很麻烦，所以一直沿用了下来。
//闰年的判断规则是：能被 4 整除但不能被 100 整除的年份为闰年，此外能被 400 整除的年份也是闰年。也就是
//例如，2000 年是闰年，有 366 天；而 1900 年不是闰年，有 365 天。
//所以 146097/400=365.2423
using Years = std::chrono::duration<Int, detail::ratio_multiply<std::ratio<DaysPer400Years, 400>, Days::period>>;
using Months = std::chrono::duration<Int, detail::ratio_divide<Years::period, std::ratio<MonthsPerYear>>>;


const std::string TicksField = "ticks";
const std::string DateDataField = "dateData";


RD_DETAIL_NAMESPACE_END

RD_TIME_NAMESPACE_END
#endif//RENDU_TIME_TIME_DATE_INCLUDE_DURATIONS_H_
