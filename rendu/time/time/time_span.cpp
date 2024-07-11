/*
* Created by boil on 2024/1/30.
*/

#include "time_span.h"
#include "time_cast.h"
#include "time_floor.h"

RD_TIME_NAMESPACE_BEGIN

TimeSpan::TimeSpan(Long microseconds)
    : m_duration{microseconds} {
}

TimeSpan::TimeSpan(Int days, Int hours, Int minutes, Int seconds, Long milliseconds)
    : m_duration(detail::Days{days} + detail::Hours{hours} + detail::Minutes{minutes} + detail::Seconds{seconds} + detail::Milliseconds{milliseconds}) {
}

Int TimeSpan::totalDays() const {
  return DurationCast<detail::Days>(m_duration).count();
}

Int TimeSpan::totalHours() const {
  return DurationCast<detail::Hours>(m_duration).count();
}

Int TimeSpan::totalMinutes() const {
  return DurationCast<detail::Minutes>(m_duration).count();
}

Int TimeSpan::totalSeconds() const {
  return DurationCast<detail::Seconds>(m_duration).count();
}

Long TimeSpan::totalMilliseconds() const {
  return DurationCast<detail::Milliseconds>(m_duration).count();
}

Double TimeSpan::totalMicroseconds() const {
  return m_duration.count();
}

Int TimeSpan::Days() const {
  return Floor<detail::Days>(m_duration).count();
}

Int TimeSpan::Hours() const {
  return Floor<detail::Hours>(m_duration).count() % detail::HoursPerDay;
}

Int TimeSpan::Minutes() const {
  return Floor<detail::Minutes>(m_duration).count() % detail::MinutesPerHour;
}

Int TimeSpan::Seconds() const {
  return Floor<detail::Seconds>(m_duration).count() % detail::SecsPerMinute;
}

Long TimeSpan::Milliseconds() const {
  return Floor<detail::Milliseconds>(m_duration).count() % detail::MillisPerSecond;
}

Double TimeSpan::Microseconds() const {
  return m_duration.count() % detail::MicrosecondsPerMillisecond;
}


TimeSpan TimeSpan::operator*(const Double &rhs) const {
  return TimeSpan(totalMicroseconds() * rhs);
}

TimeSpan TimeSpan::operator/(const Double &rhs) const {
  return TimeSpan(totalMicroseconds() / rhs);
}

TimeSpan TimeSpan::operator%(const TimeSpan &rhs) const {
  Double mod_ms = std::fmod(totalMicroseconds(), rhs.totalMicroseconds());
  return TimeSpan::fromMicroseconds(mod_ms);
}

TimeSpan TimeSpan::operator+(const TimeSpan &rhs) const {
  return TimeSpan(totalMicroseconds() + rhs.totalMicroseconds());
}

TimeSpan TimeSpan::operator-(const TimeSpan &rhs) const {
  return TimeSpan(totalMicroseconds() - rhs.totalMicroseconds());
}

TimeSpan TimeSpan::fromDays(Int days) {
  return {days};
}

TimeSpan TimeSpan::fromHours(Int hours) {
  return {0, hours};
}
TimeSpan TimeSpan::fromMinutes(Int minutes) {
  return {0, 0, minutes};
}
TimeSpan TimeSpan::fromSeconds(Int seconds) {
  return {0, 0, 0, seconds};
}
TimeSpan TimeSpan::fromMilliseconds(Long milliseconds) {
  return {0, 0, 0, 0, milliseconds};
}

TimeSpan TimeSpan::fromMicroseconds(Double microseconds) {
  return {microseconds};
}

RD_TIME_NAMESPACE_END
