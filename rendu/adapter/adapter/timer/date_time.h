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

      DateTime(std::time_t timeT, std::string timeZ = "Asia/Shanghai");

      DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, DateTimeKind kind = Local);

    public:
      static DateTime GetCurrentDateTime(std::string timeZ = "Asia/Shanghai");
    public:
      time_t GetTimeT() const;

      std::string GetTimeZone();

      std::string ToString(const std::string& timeZone = "") const;

      std::string ToUtc();

    private:
      time_t m_timeT;
      std::string m_timeZ;
    };


RD_NAMESPACE_END

#endif //RENDU_DATE_TIME_H
