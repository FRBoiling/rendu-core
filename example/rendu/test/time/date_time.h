/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_TIME_DATE_TIME_H
#define RENDU_TIME_DATE_TIME_H

#include "time_define.h"
#include "time_helper.h"

TIME_NAMESPACE_BEGIN

    enum DateTimeKind {
      Unspecified = 0,
      Utc = 1,
      Local = 2,
    };

    class DateTime  :
        public LessThanComparable<DateTime>,
        public EqualityComparable<DateTime> {
    public:
      DateTime();

      DateTime(INT64 timeS, STRING timeZ = "Asia/Shanghai");

      DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, DateTimeKind kind = Local);

    public:
      static DateTime Now(STRING timeZ = "Asia/Shanghai");

    public:
      void SetDateTime(DateTime& dateTime);

      INT64 GetTimeStamp() const;

      STRING GetTimeZone();

      STRING ToString(const STRING &timeZone = "") const;

      STRING ToUtc();

    public:
      STime m_tm;
    private:
      INT64 m_timeStamp;
      STRING m_timeZone;
    };


TIME_NAMESPACE_END

#endif //RENDU_TIME_DATE_TIME_H
