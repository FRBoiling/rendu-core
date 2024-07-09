/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_TIME_WEEK_H_
#define RENDU_TIME_WEEK_H_


#include "date_time.h"
#include "time_span.h"

TIME_NAMESPACE_BEGIN

enum class DayOfWeek {
  /// <summary>Indicates Sunday.</summary>
  Sunday,
  /// <summary>Indicates Monday.</summary>
  Monday,
  /// <summary>Indicates Tuesday.</summary>
  Tuesday,
  /// <summary>Indicates Wednesday.</summary>
  Wednesday,
  /// <summary>Indicates Thursday.</summary>
  Thursday,
  /// <summary>Indicates Friday.</summary>
  Friday,
  /// <summary>Indicates Saturday.</summary>
  Saturday,
};

class Week {
public:
  Week(const DateTime &start):m_start(start){};
private:
  DateTime m_start;

public:
  DateTime Start() const {return m_start;}
  DateTime End() const {return m_start + TimeSpan::FromDays(6);}

  bool CheckInWeek(const DateTime &date){
    return date >= Start() && date <= End();
  }

public:
  static Week FromDate(const DateTime &date) {
    return Week{Start(date)};
  }

  static DayOfWeek GetDayOfWeek(const DateTime &date) {
    return (DayOfWeek)date.DayOfWeek();
  }

  static DateTime Start(const DateTime &date) {
    int day_of_week = date.DayOfWeek();
    return date - TimeSpan::FromDays(day_of_week - 1);
  }

  static DateTime End(const DateTime &date)  {
    int day_of_week = date.DayOfWeek();
    return date + TimeSpan::FromDays(7 - day_of_week);
  }

  static bool CheckInSameWeek(const DateTime &date1,const DateTime &date2) {
    return Start(date1) == Start(date2);
  }

};

TIME_NAMESPACE_END

#endif//RENDU_TIME_WEEK_H_
