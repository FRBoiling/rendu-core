/*
* Created by boil on 2023/10/26.
*/

#include "local_time.h"

RD_NAMESPACE_BEGIN

  static int _daylight_active;
  static long _current_timezone;

  void LocalTimeInit() {
    /* Obtain timezone and daylight info. */
    tzset(); /* Now 'timezome' global is populated. */
#if defined(__linux__) || defined(__sun)
    _current_timezone  = timezone;
#elif defined(_WIN32)
    time_t time_utc;
  struct tm tm_local;

  // Get the UTC time
  time(&time_utc);

  // Get the local time
  // Use localtime_r for threads safe for linux
  //localtime_r(&time_utc, &tm_local);
  localtime_s(&tm_local, &time_utc);

  time_t time_local;
  struct tm tm_gmt;

  // Change tm to time_t
  time_local = mktime(&tm_local);

  // Change it to GMT tm
  //gmtime_r(&time_utc, &tm_gmt);//linux
  gmtime_s(&tm_gmt, &time_utc);

  int time_zone = tm_local.tm_hour - tm_gmt.tm_hour;
  if (time_zone < -12) {
    time_zone += 24;
  }
  else if (time_zone > 12) {
    time_zone -= 24;
  }

    _current_timezone = time_zone;
#else
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    _current_timezone = tz.tz_minuteswest * 60L;
#endif
    time_t t = time(NULL);
    struct tm *aux = localtime(&t);
    _daylight_active = aux->tm_isdst;
  }


  int GetDaylightActive() {
    return _daylight_active;
  }

  static int is_leap_year(time_t year) {
    if (year % 4)
      return 0; /* A year not divisible by 4 is not leap. */
    else if (year % 100)
      return 1; /* If div by 4 and not 100 is surely leap. */
    else if (year % 400)
      return 0; /* If div by 100 *and* not by 400 is not leap. */
    else
      return 1; /* If div by 100 and 400 is leap. */
  }

  void NoLocksLocalTime(struct tm *tmp, time_t t) {
    const time_t secs_min = 60;
    const time_t secs_hour = 3600;
    const time_t secs_day = 3600 * 24;

    t -= _current_timezone; /* Adjust for timezone. */
    t += 3600 * GetDaylightActive(); /* Adjust for daylight time. */
    time_t days = t / secs_day; /* Days passed since epoch. */
    time_t seconds = t % secs_day; /* Remaining seconds. */

    tmp->tm_isdst = GetDaylightActive();
    tmp->tm_hour = seconds / secs_hour;
    tmp->tm_min = (seconds % secs_hour) / secs_min;
    tmp->tm_sec = (seconds % secs_hour) % secs_min;
#ifndef _WIN32
    tmp->tm_gmtoff = -_current_timezone;
#endif
    /* 1/1/1970 was a Thursday, that is, day 4 from the POV of the tm structure
     * where sunday = 0, so to calculate the day of the week we have to add 4
     * and take the modulo by 7. */
    tmp->tm_wday = (days + 4) % 7;

    /* Calculate the current year. */
    tmp->tm_year = 1970;
    while (1) {
      /* Leap years have one day more. */
      time_t days_this_year = 365 + is_leap_year(tmp->tm_year);
      if (days_this_year > days)
        break;
      days -= days_this_year;
      tmp->tm_year++;
    }
    tmp->tm_yday = days; /* Number of day of the current year. */

    /* We need to calculate in which month and day of the month we are. To do
     * so we need to skip days according to how many days there are in each
     * month, and adjust for the leap year that has one more day in February. */
    int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    mdays[1] += is_leap_year(tmp->tm_year);

    tmp->tm_mon = 0;
    while (days >= mdays[tmp->tm_mon]) {
      days -= mdays[tmp->tm_mon];
      tmp->tm_mon++;
    }

    tmp->tm_mday = days + 1; /* Add 1 since our 'days' is zero-based. */
    tmp->tm_year -= 1900; /* Surprisingly tm_year is year-1900. */
  }



RD_NAMESPACE_END