/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_TIME_EXAMPLE_H
#define RENDU_TIME_EXAMPLE_H

#include "time.hpp"
#include "log/log.hpp"

using namespace rendu::time;
using namespace rendu::log;

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

  // RD_INFO("now: America/New_York {}", now.ToString("America/New_York"));
  // RD_INFO("now: Europe/Paris {}", now.ToString("Europe/Paris"));

  //  // 创建一个绑定到纽约时区的zoned_time
  //  zoned_time newYorkTz{ "America/New_York", now };
  //
  //  std::cout << newYorkTz << std::endl;
  //
  //  // 创建一个绑定到巴黎时区的zoned_time
  //  zoned_time parisTz{ "Europe/Paris", now };
  //
  //  // Get the current time as a `std::chrono::time_point`
  //  auto tp = system_clock::now();
  //
  //  // Convert it to a `std::time_t`
  //  auto t = system_clock::to_time_t(tp);
  //
  //  // Convert `std::time_t` to `std::tm` in local time zone
  //  tm local_tm = *localtime(&t);
  //  auto local_time = system_clock::from_time_t(mktime(&local_tm));
  //
  //  // Get the time duration since epoch in hours
  //  auto hours = duration_cast<std::chrono::hours>(local_time.time_since_epoch());
  //
  //  // Create a UTC `std::tm` structure
  //  tm utc_tm = *gmtime(&t);
  //  auto utc_time = system_clock::from_time_t(mktime(&utc_tm));
  //
  //  // Get the time duration since epoch in hours (UTC)
  //  auto utc_hours = duration_cast<std::chrono::hours>(utc_time.time_since_epoch());
  //
  ////  // Calculate the difference between local time and UTC time
  ////  auto diff_hours = (hours.count() - utc_hours.count());
  ////  string tz_offset = format("{:%z}", local_tm);
  ////  cout << "TimeZone offset is GMT"<< (diff_hours >= 0 ? "+" : "") << diff_hours << endl;
  return 0;
}


#endif//RENDU_TIME_EXAMPLE_H
