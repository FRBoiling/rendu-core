/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_TIME_TIME_YEAR_YEAR_H_
#define RENDU_TIME_TIME_YEAR_YEAR_H_

#include "type_fwd.h"

RD_TIME_NAMESPACE_BEGIN

class Year {
private:
  short y_;

public:
  Year() = default;
  explicit constexpr Year(int y) noexcept;

  constexpr Year &operator++() noexcept;
  constexpr Year operator++(int) noexcept;
  constexpr Year &operator--() noexcept;
  constexpr Year operator--(int) noexcept;

  constexpr Year &operator+=(const detail::Years &y) noexcept;
  constexpr Year &operator-=(const detail::Years &y) noexcept;

  constexpr Year operator-() const noexcept;
  constexpr Year operator+() const noexcept;

  constexpr bool is_leap() const noexcept;

  constexpr explicit operator int() const noexcept;
  constexpr bool ok() const noexcept;

  static constexpr Year min() noexcept { return Year{-32767}; }
  static constexpr Year max() noexcept { return Year{32767}; }
};

constexpr bool operator==(const Year& x, const Year& y) noexcept;
constexpr bool operator!=(const Year& x, const Year& y) noexcept;
constexpr bool operator< (const Year& x, const Year& y) noexcept;
constexpr bool operator> (const Year& x, const Year& y) noexcept;
constexpr bool operator<=(const Year& x, const Year& y) noexcept;
constexpr bool operator>=(const Year& x, const Year& y) noexcept;

constexpr Year  operator+(const Year&  x, const detail::Years& y) noexcept;
constexpr Year  operator+(const detail::Years& x, const Year&  y) noexcept;
constexpr Year  operator-(const Year&  x, const detail::Years& y) noexcept;
constexpr detail::Years operator-(const Year&  x, const Year&  y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&operator<<(std::basic_ostream<CharT, Traits>& os, const Year& y);


RD_TIME_NAMESPACE_END

#include "year.inl"

#endif//RENDU_TIME_TIME_YEAR_YEAR_H_
