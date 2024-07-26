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
  std::cout << "初始化的 std::chrono::system_clock::time_point: " << tp << std::endl;

  DateTime dt(year, month, day, hour, minute, second, millisecond);
  std::cout << "初始化的 DateTime: " << dt.ToString() << std::endl;
  std::cout << "初始化的 DateTime: " << dt << std::endl;

  auto now = DateTime::Now();
  std::cout << "当前时间: " << now << std::endl;
}

int getYearFromTimePoint(std::chrono::system_clock::time_point timePoint) {
  std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
  struct tm *timeInfo = std::localtime(&timeT);
  return timeInfo->tm_year + 1900;
}

int date_time_example() {
  std::cout << "------------date_time_example--------- " << std::endl;
  using namespace std::chrono;
  auto tp = system_clock::now();
  std::cout << "system_clock now: " << tp << std::endl;
  using namespace rendu::time;
  //  DateTime now = DateTime::Now(DateTime::Kind::Utc);
  //  DateTime now{tp, DateTime::Kind::Utc};
  DateTime now{tp};
  SPDLOG_ERROR("DateTime now: {}", now.ToString());
  auto sc = steady_clock::now();
  std::cout << "steady_clock now: "  << std::endl;

  ////  auto tp = zoned_time{current_zone(), system_clock::now()}.get_local_time();
  auto dp = floor<days>(tp);
  year_month_day ymd{dp};
  hh_mm_ss time{floor<milliseconds>(tp - dp)};
  auto y = ymd.year();
  auto m = ymd.month();
  auto d = ymd.day();
  auto h = time.hours();
  auto M = time.minutes();
  auto s = time.seconds();
  auto ms = time.subseconds();
  std::cout << "year: " << y << std::endl;
  std::cout << "month: " << m << std::endl;
  std::cout << "day: " << d << std::endl;
  std::cout << "hour: " << h << std::endl;
  std::cout << "minutes: " << M << std::endl;
  std::cout << "seconds: " << s << std::endl;
  std::cout << "millisecond: " << ms.count() << std::endl;

  SPDLOG_ERROR("year: {}", now.Year());
  SPDLOG_ERROR("month: {}", now.Month());
  SPDLOG_ERROR("day: {}", now.Day());
  SPDLOG_ERROR("hour: {}", now.Hour());
  SPDLOG_ERROR("minutes: {}", now.Minute());
  SPDLOG_ERROR("seconds: {}", now.Second());
  SPDLOG_ERROR("millisecond: {}", now.Millisecond());

  TimeSpan timeSpan = TimeSpan::fromDays(1);
  DateTime date1 = now.AddDays(2) - timeSpan;
  DateTime date2 = now.AddDays(2) + timeSpan;
  SPDLOG_ERROR("date1: {}", date1.ToString());
  SPDLOG_ERROR("date2: {}", date2.ToString());

  std::cout << "now11111: " << now << std::endl;
  auto date_time = now.AddYears(1);
  SPDLOG_ERROR("AddYears date_time: {}", date_time.ToString());
  date_time = now.AddMonths(detail::MonthsPerYear-1);
  SPDLOG_ERROR("AddMonths date_time: {}", date_time.ToString());
  date_time = now.AddDays(detail::MonthsPerYear * 30);
  SPDLOG_ERROR("AddDays date_time: {}", date_time.ToString());
  date_time = now.AddHours(detail::HoursPerDay);
  SPDLOG_ERROR("AddHours date_time: {}", date_time.ToString());
  date_time = now.AddMinutes(detail::MinutesPerHour * detail::HoursPerDay);
  SPDLOG_ERROR("AddMinutes date_time: {}", date_time.ToString());


  //      if (Week::CheckInSameWeek(date1, date2)) {
  //        SPDLOG_ERROR("date1 and date2 are in the same week");
  //      } else {
  //        SPDLOG_ERROR("date1 and date2 are not in the same week");
  //      }

  return 0;
}

int time_zone_example() {
  //  std::cout << "------------time_zone_example--------- " << std::endl;
  //  using namespace std::chrono;
  //  // 假设当前时间（这里仅作示例，实际应使用 system_clock::now()）
  //  sys_time<seconds> now = sys_time<seconds>{};
  //  // 获取本地时区
  //  auto z = std::chrono::current_zone();
  //  zoned_time<seconds> zt{z, floor<seconds>(now)};
  //
  //  // 打印本地时间
  //  std::cout << "本地时间: " << std::format("{:%Y-%m-%d %H:%M:%S %Z}", zt) << std::endl;
  //  //  auto zone = TimeZoneInfo::Local();
  //  //  SPDLOG_ERROR("zone: {}", zone.Id());
  //  //  SPDLOG_ERROR("zone: {}", zone.DisplayName());
  //  //  SPDLOG_ERROR("zone: {}", zone.StandardName());
  //  //  SPDLOG_ERROR("zone: {}", zone.DaylightName());
  //  //  SPDLOG_ERROR("zone: {}", zone.BaseUtcOffset().count());
  //  //  SPDLOG
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

  //  duration_example();
  //  time_span_example();
    date_time_example();
  //  time_zone_example();
}


#endif//RENDU_TIME_EXAMPLE_H
