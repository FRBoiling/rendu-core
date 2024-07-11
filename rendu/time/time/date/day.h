/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_TIME_TIME_DATE_DAY_H_
#define RENDU_TIME_TIME_DATE_DAY_H_

#include "type_fwd.h"

RD_TIME_NAMESPACE_BEGIN

class Day {
private:
  unsigned char d_;

public:
  Day() = default;
  explicit constexpr Day(unsigned d) noexcept;

  constexpr Day& operator++()    noexcept;
  constexpr Day  operator++(int) noexcept;
  constexpr Day& operator--()    noexcept;
  constexpr Day  operator--(int) noexcept;

  constexpr Day& operator+=(const detail::Days& d) noexcept;
  constexpr Day& operator-=(const detail::Days& d) noexcept;

  constexpr explicit operator unsigned() const noexcept;
  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const Day& x, const Day& y) noexcept;
constexpr bool operator!=(const Day& x, const Day& y) noexcept;
constexpr bool operator< (const Day& x, const Day& y) noexcept;
constexpr bool operator> (const Day& x, const Day& y) noexcept;
constexpr bool operator<=(const Day& x, const Day& y) noexcept;
constexpr bool operator>=(const Day& x, const Day& y) noexcept;

constexpr Day  operator+(const Day&  x, const detail::Days& y) noexcept;
constexpr Day  operator+(const detail::Days& x, const Day&  y) noexcept;
constexpr Day  operator-(const Day&  x, const detail::Days& y) noexcept;
constexpr detail::Days operator-(const Day&  x, const Day&  y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const Day& d);

RD_TIME_NAMESPACE_END

#include "day.inl"

#endif//RENDU_TIME_TIME_DATE_DAY_H_
