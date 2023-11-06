/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_COMMON_DATE_TIME_H
#define RENDU_COMMON_DATE_TIME_H

#include "common/define.h"

COMMON_NAMESPACE_BEGIN

    enum DateTimeKind {
      Unspecified = 0,
      Utc = 1,
      Local = 2,
    };

    class DateTime {
    public:
      DateTime();

      DateTime(INT64 timeS, STRING timeZ = "Asia/Shanghai");

      DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, DateTimeKind kind = Local);

    public:
      static DateTime GetCurrentDateTime(STRING timeZ = "Asia/Shanghai");

    public:
      void SetDateTime(DateTime& dateTime);

      INT64 GetTimeStamp() const;

      STRING GetTimeZone();

      STRING ToString(const STRING &timeZone = "") const;

      STRING ToUtc();

    public:
      STIME m_tm;
    private:
      INT64 m_timeStamp;
      STRING m_timeZone;
    };


COMMON_NAMESPACE_END

#endif //RENDU_COMMON_DATE_TIME_H
