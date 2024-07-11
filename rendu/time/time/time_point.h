/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_INCLUDE_TIME_POINT_H_
#define RENDU_TIME_TIME_DATE_INCLUDE_TIME_POINT_H_

#include "time_duration.h"

RD_TIME_NAMESPACE_BEGIN
RD_DETAIL_NAMESPACE_BEGIN

using SysClock = std::chrono::system_clock;
using SysTimePoint = SysClock::time_point;

template<class Duration>
using SysTime = std::chrono::time_point<SysClock, Duration>;

using SysMilliSeconds = SysTime<Milliseconds>;
using SysSeconds = SysTime<Seconds>;
using SysMinutes = SysTime<Minutes>;
using SysHours = SysTime<Hours>;
using SysDays = SysTime<Days>;

struct local_t {};

template<class Duration>
using LocalTime = std::chrono::time_point<local_t, Duration>;

using LocalSeconds = LocalTime<Seconds>;
using LocalMilliSeconds = LocalTime<Milliseconds>;
using LocalSeconds = LocalTime<Seconds>;
using LocalMinutes = LocalTime<Minutes>;
using LocalHours = LocalTime<Hours>;
using LocalDays = LocalTime<Days>;

RD_DETAIL_NAMESPACE_END
RD_TIME_NAMESPACE_END
#endif//RENDU_TIME_TIME_DATE_INCLUDE_TIME_POINT_H_
