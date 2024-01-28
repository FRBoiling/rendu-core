/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_COMMON_DATE_TIME_H
#define RENDU_COMMON_DATE_TIME_H

#include "common_define.h"

COMMON_NAMESPACE_BEGIN

    enum DateTimeKind {
      Unspecified = 0,
      Utc = 1,
      Local = 2,
    };

    class DateTime {
    public:
      DateTime();

      DateTime(std::int64_t timeS, std::string timeZ = "Asia/Shanghai");

      DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, DateTimeKind kind = Local);

    public:
      static DateTime GetCurrentDateTime(std::string timeZ = "Asia/Shanghai");

    public:
      void SetDateTime(DateTime& dateTime);

      std::int64_t GetTimeStamp() const;

      std::string GetTimeZone();

      std::string ToString(const std::string &timeZone = "") const;

      std::string ToUtc();

    public:
      STime m_tm;
    private:
      std::int64_t m_timeStamp;
      std::string m_timeZone;
    };


COMMON_NAMESPACE_END

#endif //RENDU_COMMON_DATE_TIME_H
