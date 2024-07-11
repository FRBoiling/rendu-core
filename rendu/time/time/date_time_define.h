/*
* Created by boil on 2024/7/13.
*/

#ifndef RENDU_TIME_TIME_DATE_TIME_DEFINE_H_
#define RENDU_TIME_TIME_DATE_TIME_DEFINE_H_

#include "time_point.h"

RD_TIME_NAMESPACE_BEGIN
RD_DETAIL_NAMESPACE_BEGIN
using Year = std::chrono::year;
using Month = std::chrono::month;
using Day = std::chrono::day;

using YearMonthDay = std::chrono::year_month_day;
RD_DETAIL_NAMESPACE_END
RD_TIME_NAMESPACE_END



#endif//RENDU_TIME_TIME_DATE_TIME_DEFINE_H_
