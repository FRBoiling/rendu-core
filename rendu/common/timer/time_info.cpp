/*
* Created by boil on 2023/9/21.
*/

#include "time_info.h"

RD_NAMESPACE_BEGIN

    TimeInfo::TimeInfo() : m_dt(0), m_timeZone(0), m_dt1970(0) {}

    TimeInfo::~TimeInfo() = default;

    void TimeInfo::SetTimeZone(int timeZone) {
      m_timeZone = timeZone;
    }
    int TimeInfo::GetTimeZone() const {
      return m_timeZone;
    }

    void TimeInfo::SetTimeT(time_t &tt) {
      m_dt = DateTime(tt);
    }

    time_t TimeInfo::GetTimeT() {
      return m_dt.GetTimeT();
    }

    void TimeInfo::SetDt(DateTime &dt) {
      m_dt = dt;
    }

    DateTime TimeInfo::GetDt() {
      return m_dt;
    }

    void TimeInfo::SetDt1970(DateTime &dt) {
      m_dt1970 = dt;
    }

    DateTime TimeInfo::GetDt1970() {
      return m_dt1970;
    }

    long TimeInfo::GetServerMinusClientTime() {
      return 0;
    }

    long TimeInfo::GetFrameTime() {
      return 0;
    }

    long TimeInfo::GetClientNow() {
      return 0;
    }

    long TimeInfo::GetServerNow() {
      return 0;
    }

    long TimeInfo::GetClientFrameTime() {
      return 0;
    }

    long TimeInfo::GetServerFrameTime() {
      return 0;
    }




RD_NAMESPACE_END
