/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_TIME_TIME_MONTH_MONTH_H_
#define RENDU_TIME_TIME_MONTH_MONTH_H_

#include "type_fwd.h"

RD_TIME_NAMESPACE_BEGIN

class Month {

private:
  unsigned char m_;

public:
  Month() = default;
  explicit constexpr Month(unsigned m) noexcept;

  constexpr Month &operator++() noexcept;
  constexpr Month operator++(int) noexcept;
  constexpr Month &operator--() noexcept;
  constexpr Month operator--(int) noexcept;

  constexpr Month &operator+=(const detail::Months &m) noexcept;
  constexpr Month &operator-=(const detail::Months &m) noexcept;

  constexpr explicit operator unsigned() const noexcept;
  constexpr bool ok() const noexcept;
};

constexpr bool operator==(const Month &x, const Month &y) noexcept;
constexpr bool operator!=(const Month &x, const Month &y) noexcept;
constexpr bool operator<(const Month &x, const Month &y) noexcept;
constexpr bool operator>(const Month &x, const Month &y) noexcept;
constexpr bool operator<=(const Month &x, const Month &y) noexcept;
constexpr bool operator>=(const Month &x, const Month &y) noexcept;

constexpr Month operator+(const Month &x, const detail::Months &y) noexcept;
constexpr Month operator+(const detail::Months &x, const Month &y) noexcept;
constexpr Month operator-(const Month &x, const detail::Months &y) noexcept;
constexpr detail::Months operator-(const Month &x, const Month &y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const Month &m);

RD_DETAIL_NAMESPACE_BEGIN
struct unspecified_month_disambiguator {};
RD_DETAIL_NAMESPACE_END


RD_TIME_NAMESPACE_END

#include "month.inl"

#endif//RENDU_TIME_TIME_MONTH_MONTH_H_
