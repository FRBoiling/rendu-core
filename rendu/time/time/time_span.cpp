/*
* Created by boil on 2024/1/30.
*/

#include "time_span.h"

TIME_NAMESPACE_BEGIN

TimeSpan::TimeSpan(std::int64_t ticks)
    : m_duration(std::chrono::milliseconds{ticks / TicksPerMillisecond}) {}

TimeSpan::TimeSpan(INT32 days, INT32 hours, INT32 minutes, INT32 seconds, INT64 milliseconds)
    : m_duration(date::days{days} + std::chrono::hours{hours} + std::chrono::minutes{minutes} + std::chrono::seconds{seconds} + std::chrono::milliseconds{milliseconds}) {}

INT32 TimeSpan::TotalDays() const {
  return duration_cast<date::days>(m_duration.time_since_epoch()).count();
}

INT32 TimeSpan::TotalHours() const {
  return duration_cast<std::chrono::hours>(m_duration.time_since_epoch()).count();
}

INT32 TimeSpan::TotalMinutes() const {
  return duration_cast<std::chrono::minutes>(m_duration.time_since_epoch()).count();
}

INT32 TimeSpan::TotalSeconds() const {
  return duration_cast<std::chrono::seconds>(m_duration.time_since_epoch()).count();
}

INT64 TimeSpan::TotalMilliseconds() const {
  return m_duration.time_since_epoch().count();
}

TimeSpan::Rep TimeSpan::Ticks() const {
  return m_duration.time_since_epoch().count();
}


INT32 TimeSpan::Days() const {
  return date::floor<date::days>(m_duration.time_since_epoch()).count();
}

INT32 TimeSpan::Hours() const {
  return date::floor<std::chrono::hours>(m_duration.time_since_epoch()).count() % HoursPerDay;
}

INT32 TimeSpan::Minutes() const {
  return date::floor<std::chrono::minutes>(m_duration.time_since_epoch()).count() % MinutesPerHour;
}

INT32 TimeSpan::Seconds() const {
  return date::floor<std::chrono::seconds>(m_duration.time_since_epoch()).count() % SecsPerMinute;
}

INT64 TimeSpan::Milliseconds() const {
  return m_duration.time_since_epoch().count() % MillisPerSecond;
}

TimeSpan TimeSpan::operator*(const double &rhs) const {
  return TimeSpan(0, 0, 0, 0, (Rep) (TotalMilliseconds() * rhs));
}

TimeSpan TimeSpan::operator/(const double &rhs) const {
  return TimeSpan(0, 0, 0, 0, (Rep) (TotalMilliseconds() / rhs));
}

TimeSpan TimeSpan::operator%(const TimeSpan &rhs) const {
  double mod_ms = std::fmod(TotalMilliseconds(), rhs.TotalMilliseconds());
  return TimeSpan::FromMilliseconds(mod_ms);
}

TimeSpan TimeSpan::FromDays(INT32 days) {
  return {days};
}

TimeSpan TimeSpan::FromHours(INT32 hours){
  return {0,hours};

}
TimeSpan TimeSpan::FromMinutes(INT32 minutes){
  return {0,0,minutes};

}
TimeSpan TimeSpan::FromSeconds(INT32 seconds){
  return {0,0,0,seconds};

}
TimeSpan TimeSpan::FromMilliseconds(INT64 milliseconds){
  return {0,0,0,0,milliseconds};
}

TIME_NAMESPACE_END
