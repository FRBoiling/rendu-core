/*
* Created by boil on 2024/1/30.
*/

#include "time_span.h"

TIME_NAMESPACE_BEGIN

TimeSpan::TimeSpan(std::int64_t ticks)
    : m_duration(std::chrono::milliseconds{ticks / TicksPerMillisecond}) {}

TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, Long milliseconds)
    : m_duration(date::days{days} + std::chrono::hours{hours} + std::chrono::minutes{minutes} + std::chrono::seconds{seconds} + std::chrono::milliseconds{milliseconds}) {}

int TimeSpan::TotalDays() const {
  return duration_cast<date::days>(m_duration.time_since_epoch()).count();
}

int TimeSpan::TotalHours() const {
  return duration_cast<std::chrono::hours>(m_duration.time_since_epoch()).count();
}

int TimeSpan::TotalMinutes() const {
  return duration_cast<std::chrono::minutes>(m_duration.time_since_epoch()).count();
}

int TimeSpan::TotalSeconds() const {
  return duration_cast<std::chrono::seconds>(m_duration.time_since_epoch()).count();
}

Long TimeSpan::TotalMilliseconds() const {
  return m_duration.time_since_epoch().count();
}

TimeSpan::Rep TimeSpan::Ticks() const {
  return m_duration.time_since_epoch().count();
}


int TimeSpan::Days() const {
  return date::floor<date::days>(m_duration.time_since_epoch()).count();
}

int TimeSpan::Hours() const {
  return date::floor<std::chrono::hours>(m_duration.time_since_epoch()).count() % HoursPerDay;
}

int TimeSpan::Minutes() const {
  return date::floor<std::chrono::minutes>(m_duration.time_since_epoch()).count() % MinutesPerHour;
}

int TimeSpan::Seconds() const {
  return date::floor<std::chrono::seconds>(m_duration.time_since_epoch()).count() % SecsPerMinute;
}

Long TimeSpan::Milliseconds() const {
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

TimeSpan TimeSpan::FromDays(int days) {
  return {days};
}

TimeSpan TimeSpan::FromHours(int hours){
  return {0,hours};

}
TimeSpan TimeSpan::FromMinutes(int minutes){
  return {0,0,minutes};

}
TimeSpan TimeSpan::FromSeconds(int seconds){
  return {0,0,0,seconds};

}
TimeSpan TimeSpan::FromMilliseconds(Long milliseconds){
  return {0,0,0,0,milliseconds};
}

TIME_NAMESPACE_END
