/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_TIME_EXAMPLE_H
#define RENDU_TIME_EXAMPLE_H

#include "log/log.hpp"
#include "time/time.hpp"

using namespace rendu::log;
using namespace rendu::time;

int time_example() {
  DateTime now = DateTime::Now();
  RD_INFO(" The current date and time is {}", now.ToString());

  RD_INFO(" The current Year is {}", now.Year());
  RD_INFO(" The current Month is {}", now.Month());
  RD_INFO(" The current Hour is {}", now.Hour());
  RD_INFO(" The current Minute is {}", now.Minute());
  RD_INFO(" The current Second is {}", now.Second());
  RD_INFO(" The current MilliSecond is {}", now.MilliSecond());
  RD_INFO(" The current DayOfWeek is {}", now.DayOfWeek());
  RD_INFO(" The current DayOfYear is {}", now.DayOfYear());

  TimeSpan timeSpan = TimeSpan::FromDays(1);
  DateTime date1 = now.AddDays(2) - timeSpan;
  DateTime date2 = now.AddDays(2) + timeSpan;

  if (Week::CheckInSameWeek(date1, date2)) {
    RD_INFO("date1 and date2 are in the same week");
  } else {
    RD_INFO("date1 and date2 are not in the same week");
  }

  return 0;
}



#endif//RENDU_TIME_EXAMPLE_H
