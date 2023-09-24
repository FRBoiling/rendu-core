/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_DATE_TIME_H
#define RENDU_DATE_TIME_H

#include "define.h"
#include <iostream>

RD_NAMESPACE_BEGIN

    enum DateTimeKind {
      Unspecified = 0,
      Utc = 1,
      Local = 2,
    };

    class DateTime {
    public:
      DateTime();

      DateTime(int64_t timeS, std::string timeZ = "Asia/Shanghai");

      DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, DateTimeKind kind = Local);

    public:
      static DateTime GetCurrentDateTime(std::string timeZ = "Asia/Shanghai");

    public:
      void SetDateTime(DateTime& dateTime);

      int64_t GetTimeStamp() const;

      std::string GetTimeZone();

      std::string ToString(const std::string &timeZone = "") const;

      std::string ToUtc();

    private:
      int64_t m_timeStamp;
      std::string m_timeZone;
    };


RD_NAMESPACE_END

#endif //RENDU_DATE_TIME_H
