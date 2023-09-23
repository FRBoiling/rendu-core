/*
* Created by boil on 2023/9/22.
*/

#include "date_time.h"
#include "exception/throw_helper.h"
#include <date/tz.h>
#include <chrono>
#include <iomanip>
#include <utility>

RD_NAMESPACE_BEGIN

//    DateTime::DateTime() : m_timeZ(date::current_zone()->name()), m_timeT(0) {
    DateTime::DateTime() : m_timeZ("Asia/Shanghai"), m_timeT(0) {
    }

    DateTime::DateTime(std::time_t timeT, std::string timeZ):m_timeT(timeT),m_timeZ(std::move(timeZ)) {

    }

    DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, DateTimeKind kind)
        : DateTime() {

      std::tm timeInfo{};
      timeInfo.tm_year = year;
      timeInfo.tm_mon = month;
      timeInfo.tm_mday = day;
      timeInfo.tm_hour = hour;
      timeInfo.tm_min = minute;
      timeInfo.tm_sec = second;
      m_timeT = std::mktime(&timeInfo);
    }

    time_t DateTime::GetTimeT() const {
      return m_timeT;
    }

     std::string DateTime::GetTimeZone(){
      return m_timeZ;
    }

    std::string DateTime::ToString(const std::string& timeZone) const {
      std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::from_time_t(m_timeT);
      std::string temp_timeZ = timeZone;
      if (temp_timeZ.empty()){
        temp_timeZ = m_timeZ;
      }
      auto target_zt = date::make_zoned(temp_timeZ, timePoint);
      std::ostringstream oss;
      oss << target_zt;
      return oss.str();
    }

    DateTime DateTime::GetCurrentDateTime(std::string timeZ) {
      auto current_time = std::chrono::system_clock::now();
      auto current_time_t = std::chrono::system_clock::to_time_t(current_time);
      return {current_time_t,std::move(timeZ)};
    }

//    DateTime& DateTime::ConvertToTimeZone(const std::string &timeZone) const {
//      auto timePointTp = std::chrono::time_point_cast<std::chrono::seconds>(m_timeT);
//      auto timeZonePtr = date::locate_zone(timeZone);
//      auto zonedTime = date::make_zoned(timeZonePtr, timePointTp);
//      auto convertedTimePoint = zonedTime.get_sys_time();
//      return this;
//    }


RD_NAMESPACE_END
