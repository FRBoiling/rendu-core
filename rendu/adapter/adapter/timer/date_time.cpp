/*
* Created by boil on 2023/9/22.
*/

#include "date_time.h"
#include <date/tz.h>
#include <chrono>
#include <iomanip>
#include <utility>

RD_NAMESPACE_BEGIN

//    DateTime::DateTime() : m_timeZ(date::current_zone()->name()), m_timeT(0) {
    DateTime::DateTime() : m_timeZone("Asia/Shanghai"), m_timeStamp(0) {
    }

    DateTime::DateTime(int64_t timeS, std::string timeZ) : m_timeStamp(timeS), m_timeZone(std::move(timeZ)) {
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
      auto time_point = std::chrono::system_clock::from_time_t(std::mktime(const_cast<std::tm*>(&timeInfo)));
      auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
      m_timeStamp = milliseconds.count();
    }

    int64_t DateTime::GetTimeStamp() const {
      return m_timeStamp;
    }

    std::string DateTime::GetTimeZone() {
      return m_timeZone;
    }

    std::string DateTime::ToString(const std::string &timeZone) const {
//      std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::from_time_t(m_timeStamp);
      auto timePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(std::chrono::milliseconds(m_timeStamp));
      std::string temp_timeZ = timeZone;
      if (temp_timeZ.empty()) {
        temp_timeZ = m_timeZone;
      }
      auto target_zt = date::make_zoned(temp_timeZ, timePoint);
      std::ostringstream oss;
      oss << target_zt;
      return oss.str();
    }

    DateTime DateTime::GetCurrentDateTime(std::string timeZ) {
      auto now = std::chrono::system_clock::now();
      auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
//      auto time_t = std::chrono::system_clock::to_time_t(now);
      auto time_ms = now_ms.time_since_epoch().count();
      return {time_ms, std::move(timeZ)};
    }

    void DateTime::SetDateTime(DateTime& dateTime) {
      m_timeStamp = dateTime.GetTimeStamp();
      m_timeZone = dateTime.GetTimeZone();
    }

//    DateTime& DateTime::ConvertToTimeZone(const std::string &timeZone) const {
//      auto timePointTp = std::chrono::time_point_cast<std::chrono::seconds>(m_timeT);
//      auto timeZonePtr = date::locate_zone(timeZone);
//      auto zonedTime = date::make_zoned(timeZonePtr, timePointTp);
//      auto convertedTimePoint = zonedTime.get_sys_time();
//      return this;
//    }


RD_NAMESPACE_END
