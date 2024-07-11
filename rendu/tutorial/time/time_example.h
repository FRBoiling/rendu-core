/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_TIME_EXAMPLE_H
#define RENDU_TIME_EXAMPLE_H

#include "time/time_include.h"
#include <spdlog/spdlog.h>

int duration_example() {
  using namespace std;
  using namespace std::chrono;
  using namespace rendu::time;

  std::chrono::milliseconds milliseconds(1);
  std::chrono::seconds second(1);

  // 创建一个天的时间间隔
  std::chrono::days day(1);

  // 创建一个周的时间间隔
  std::chrono::weeks week(1);

  // 创建一个年的时间间隔
  std::chrono::years year(1);

  // 创建一个月的时间间隔
  std::chrono::months month(1);


  std::cout << "一天的长度：" << day << "s" << std::endl;
  std::cout << "一周的长度：" << week << "s" << std::endl;
  std::cout << "一年的长度：" << year << "s" << std::endl;
  std::cout << "一个月的长度：" << month << "s" << std::endl;
}

int time_span_example() {
  using namespace rendu::time;
  SPDLOG_ERROR("TicksPerDay {}", detail::MicrosecondsPerDay);

  std::chrono::microseconds microseconds1(1);
  std::chrono::microseconds microseconds2(2);
  // 输出时间间隔的长度
  std::cout << "1：" << microseconds1.count() << std::endl;
  std::cout << "2：" << microseconds2.count() << std::endl;

  if (microseconds1 == microseconds2) {
    SPDLOG_ERROR("microseconds1 == microseconds2");
  }
  if (microseconds1 < microseconds2) {
    SPDLOG_ERROR("microseconds1 < microseconds2");
  }
  if (microseconds1 > microseconds2) {
    SPDLOG_ERROR("microseconds1 > microseconds2");
  }
  if (microseconds1 >= microseconds2) {
    SPDLOG_ERROR("microseconds1 >= microseconds2");
  }
  if (microseconds1 <= microseconds2) {
    SPDLOG_ERROR("microseconds1 <= microseconds2");
  }

  TimeSpan time_span1(1);
  TimeSpan time_span2(2);


  if (time_span1 == time_span2) {
    SPDLOG_ERROR("time_span1 == time_span2");
  }

  if (time_span1 < time_span2) {
    SPDLOG_ERROR("time_span1 < time_span2");
  }

  if (time_span1 > time_span2) {
    SPDLOG_ERROR("time_span1 > time_span2");
  }

  if (time_span1 >= time_span2) {
    SPDLOG_ERROR("time_span1 >= time_span2");
  }

  if (time_span1 <= time_span2) {
    SPDLOG_ERROR("time_span1 <= time_span2");
  }

  auto time_span3 = time_span2 - time_span1;
  if (time_span1 == time_span3) {
    SPDLOG_ERROR("time_span1 == time_span3");
  }

  int year = 2023;
  uint month = 10;
  uint day = 1;
  int hour = 12;
  int minute = 30;
  int second = 0;
  int millisecond = 1000;

  // 使用 std::chrono::year、std::chrono::month、std::chrono::day、std::chrono::hours、std::chrono::minutes、std::chrono::seconds 和 std::chrono::milliseconds 来创建各个时间单位
  std::chrono::year y(year);
  std::chrono::month m(month);
  std::chrono::day d(day);
  std::chrono::hours h(hour);
  std::chrono::minutes min(minute);
  std::chrono::seconds s(second);
  std::chrono::milliseconds ms(millisecond);

  // 使用 std::chrono::time_point 的构造函数来创建 std::chrono::system_clock::time_point 对象
  std::chrono::system_clock::time_point tp = std::chrono::sys_days(std::chrono::year_month_day(y, m, d)) + h + min + s + ms;

  // 输出结果
  std::cout << "初始化的 std::chrono::system_clock::time_point: " << tp  << std::endl;

  DateTime dt(year, month, day, hour, minute, second,millisecond);
  std::cout << "初始化的 DateTime: " << dt.ToString()  << std::endl;
  std::cout << "初始化的 DateTime: " << dt << std::endl;

  auto now = DateTime::Now();
  std::cout << "当前时间: " << now << std::endl;

}


int time_example() {
  //  using namespace std;
  //  using namespace std::chrono;
  //  using namespace rendu::time;
  //  auto tp = system_clock::now();
  //  std::cout << tp << std::endl;
  //  auto dp = floor<days>(tp);
  //  std::cout << dp << std::endl;
  //  auto ymd = YearMonthDay{dp};
  //  std::cout << ymd << std::endl;

  duration_example();
  time_span_example();
}

int date_time_example() {
  //  DateTime now = DateTime::Now();
  //  RD_INFO(" The current date and time is {}", now.ToString());
  //
  //  RD_INFO(" The current Year is {}", now.Year());
  //  RD_INFO(" The current Month is {}", now.Month());
  //  RD_INFO(" The current Hour is {}", now.Hour());
  //  RD_INFO(" The current Minute is {}", now.Minute());
  //  RD_INFO(" The current Second is {}", now.Second());
  //  RD_INFO(" The current MilliSecond is {}", now.MilliSecond());
  //  RD_INFO(" The current DayOfWeek is {}", now.DayOfWeek());
  //  RD_INFO(" The current DayOfYear is {}", now.DayOfYear());
  //
  //  TimeSpan timeSpan = TimeSpan::FromDays(1);
  //  DateTime date1 = now.AddDays(2) - timeSpan;
  //  DateTime date2 = now.AddDays(2) + timeSpan;
  //
  //  if (Week::CheckInSameWeek(date1, date2)) {
  //    RD_INFO("date1 and date2 are in the same week");
  //  } else {
  //    RD_INFO("date1 and date2 are not in the same week");
  //  }
  //
  //  return 0;
}


#endif//RENDU_TIME_EXAMPLE_H
