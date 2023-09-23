/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_TIME_INFO_H
#define RENDU_TIME_INFO_H

#include "define.h"
#include <ctime>
#include "timer/date_time.h"

RD_NAMESPACE_BEGIN

    class TimeInfo {

    public:
      TimeInfo();

      ~TimeInfo();

    private:
      DateTime m_dt1970;
      DateTime m_dt;
      int m_timeZone;

    public:
      void SetTimeZone(int timeZone);

      int GetTimeZone() const;

      void SetTimeT(time_t &tt);

      time_t GetTimeT();

      void SetDt(DateTime &dt);

      DateTime GetDt();

      void SetDt1970(DateTime &dt);

      DateTime GetDt1970();

    public:
      long GetServerMinusClientTime();

      long GetFrameTime();

      // 线程安全
      long GetClientNow();

      long GetServerNow();

      long GetClientFrameTime();

      long GetServerFrameTime();

    };

RD_NAMESPACE_END

#endif //RENDU_TIME_INFO_H
