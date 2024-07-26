/*
* Created by boil on 2024/1/30.
*/

#include "time_span.h"

RD_TIME_NAMESPACE_BEGIN


inline Int TimeSpan::totalDays() const {
  return std::chrono::duration_cast<detail::Days>(m_duration).count();
}

inline Long TimeSpan::totalHours() const {
  return std::chrono::duration_cast<detail::Hours>(m_duration).count();
}

inline Long TimeSpan::totalMinutes() const {
  return std::chrono::duration_cast<detail::Minutes>(m_duration).count();
}

inline Long TimeSpan::totalSeconds() const {
  return std::chrono::duration_cast<detail::Seconds>(m_duration).count();
}

inline Long TimeSpan::totalMilliseconds() const {
  return std::chrono::duration_cast<detail::Milliseconds>(m_duration).count();
}

inline Int TimeSpan::Days() const {
  return std::chrono::floor<detail::Days>(m_duration).count();
}

inline Long TimeSpan::Hours() const {
  return std::chrono::floor<detail::Hours>(m_duration).count() % detail::HoursPerDay;
}

inline Long TimeSpan::Minutes() const {
  return std::chrono::floor<detail::Minutes>(m_duration).count() % detail::MinutesPerHour;
}

inline Long TimeSpan::Seconds() const {
  return std::chrono::floor<detail::Seconds>(m_duration).count() % detail::SecsPerMinute;
}

inline Long TimeSpan::Milliseconds() const {
  return std::chrono::floor<detail::Milliseconds>(m_duration).count() % detail::MillisPerSecond;
}

inline TimeSpan TimeSpan::operator*(const Long &rhs) const {
  return {totalMilliseconds() * rhs};
}

inline TimeSpan TimeSpan::operator/(const Long &rhs) const {
  return {totalMilliseconds() / rhs};
}

inline TimeSpan TimeSpan::operator%(const TimeSpan &rhs) const {
  Double mod_ms = std::fmod(totalMilliseconds(), rhs.totalMilliseconds());
  return TimeSpan::fromMilliseconds((Long)mod_ms);
}

inline TimeSpan TimeSpan::operator+(const TimeSpan &rhs) const {
  return {totalMilliseconds() + rhs.totalMilliseconds()};
}

inline TimeSpan TimeSpan::operator-(const TimeSpan &rhs) const {
  return {totalMilliseconds() - rhs.totalMilliseconds()};
}

inline TimeSpan TimeSpan::fromDays(Int days) {
  return {days,0};
}

inline TimeSpan TimeSpan::fromHours(Int hours) {
  return {0, hours};
}
inline TimeSpan TimeSpan::fromMinutes(Int minutes) {
  return {0, 0, minutes};
}
inline TimeSpan TimeSpan::fromSeconds(Int seconds) {
  return {0, 0, 0, seconds};
}
inline TimeSpan TimeSpan::fromMilliseconds(Long milliseconds) {
  return {0, 0, 0, 0, milliseconds};
}

//inline TimeSpan TimeSpan::fromMicroseconds(Long microseconds) {
//  return {microseconds};
//}

RD_TIME_NAMESPACE_END
