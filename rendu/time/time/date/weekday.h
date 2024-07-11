/*
* Created by boil on 2024/7/12.
*/

#ifndef RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_H_
#define RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_H_

#include "type_fwd.h"

RD_TIME_NAMESPACE_BEGIN

enum class DayOfWeek {
  /// <summary>Indicates Sunday.</summary>
  Sunday,
  /// <summary>Indicates Monday.</summary>
  Monday,
  /// <summary>Indicates Tuesday.</summary>
  Tuesday,
  /// <summary>Indicates Wednesday.</summary>
  Wednesday,
  /// <summary>Indicates Thursday.</summary>
  Thursday,
  /// <summary>Indicates Friday.</summary>
  Friday,
  /// <summary>Indicates Saturday.</summary>
  Saturday,
};

class WeekdayIndexed;
class WeekdayLast;

class Weekday {
private:
  unsigned char wd_;

public:
  unsigned char getWd() const { return wd_; };

public:
  Weekday() = default;
  explicit constexpr Weekday(unsigned wd) noexcept;
  constexpr Weekday(const detail::SysDays &dp) noexcept;
  constexpr explicit Weekday(const detail::LocalDays &dp) noexcept;

  constexpr Weekday &operator++() noexcept;
  constexpr Weekday operator++(int) noexcept;
  constexpr Weekday &operator--() noexcept;
  constexpr Weekday operator--(int) noexcept;

  constexpr Weekday &operator+=(const detail::Days &d) noexcept;
  constexpr Weekday &operator-=(const detail::Days &d) noexcept;

  constexpr bool ok() const noexcept;

  constexpr unsigned c_encoding() const noexcept;
  constexpr unsigned iso_encoding() const noexcept;

  constexpr WeekdayIndexed operator[](unsigned index) const noexcept;
  constexpr WeekdayLast operator[](LastSpec) const noexcept;

private:
  static constexpr unsigned char weekday_from_days(int z) noexcept;

  friend constexpr bool operator==(const Weekday &x, const Weekday &y) noexcept;
  friend constexpr detail::Days operator-(const Weekday &x, const Weekday &y) noexcept;
  friend constexpr Weekday operator+(const Weekday &x, const detail::Days &y) noexcept;
  template<class CharT, class Traits>
  friend std::basic_ostream<CharT, Traits> &
  operator<<(std::basic_ostream<CharT, Traits> &os, const Weekday &wd);
};

constexpr bool operator==(const Weekday &x, const Weekday &y) noexcept;
constexpr bool operator!=(const Weekday &x, const Weekday &y) noexcept;

constexpr Weekday operator+(const Weekday &x, const detail::Days &y) noexcept;
constexpr Weekday operator+(const detail::Days &x, const Weekday &y) noexcept;
constexpr Weekday operator-(const Weekday &x, const detail::Days &y) noexcept;
constexpr detail::Days operator-(const Weekday &x, const Weekday &y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const Weekday &wd);

RD_TIME_NAMESPACE_END

#include "weekday.inl"

#endif//RENDU_TIME_TIME_DATE_WEEK_WEEKDAY_H_
