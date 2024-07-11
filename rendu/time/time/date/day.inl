/*
* Created by boil on 2024/7/11.
*/

#include "day.h"
#include "utilities.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline Day::Day(unsigned d) noexcept
    : d_(static_cast<decltype(d_)>(d)) {
}

constexpr inline Day &Day::operator++() noexcept {
  ++d_;
  return *this;
}
constexpr inline Day Day::operator++(int) noexcept {
  auto tmp(*this);
  ++(*this);
  return tmp;
}
constexpr inline Day &Day::operator--() noexcept {
  --d_;
  return *this;
}
constexpr inline Day Day::operator--(int) noexcept {
  auto tmp(*this);
  --(*this);
  return tmp;
}
constexpr inline Day &Day::operator+=(const detail::Days &d) noexcept {
  *this = *this + d;
  return *this;
}
constexpr inline Day &Day::operator-=(const detail::Days &d) noexcept {
  *this = *this - d;
  return *this;
}
constexpr inline Day::operator unsigned() const noexcept { return d_; }
constexpr inline bool Day::ok() const noexcept { return 1 <= d_ && d_ <= 31; }

constexpr inline bool operator==(const Day &x, const Day &y) noexcept {
  return static_cast<unsigned>(x) == static_cast<unsigned>(y);
}

constexpr inline bool operator!=(const Day &x, const Day &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const Day &x, const Day &y) noexcept {
  return static_cast<unsigned>(x) < static_cast<unsigned>(y);
}

constexpr inline bool operator>(const Day &x, const Day &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const Day &x, const Day &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const Day &x, const Day &y) noexcept {
  return !(x < y);
}

constexpr inline detail::Days operator-(const Day &x, const Day &y) noexcept {
  return detail::Days{static_cast<detail::Days::rep>(static_cast<unsigned>(x) - static_cast<unsigned>(y))};
}

constexpr inline Day operator+(const Day &x, const detail::Days &y) noexcept {
  return Day{static_cast<unsigned>(x) + static_cast<unsigned>(y.count())};
}

constexpr inline Day operator+(const detail::Days &x, const Day &y) noexcept {
  return y + x;
}

constexpr inline Day operator-(const Day &x, const detail::Days &y) noexcept {
  return x + -y;
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const Day &d) {
  detail::save_ostream<CharT, Traits> _(os);
  os.fill('0');
  os.flags(std::ios::dec | std::ios::right);
  os.width(2);
  os << static_cast<unsigned>(d);
  return os;
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const Day &d) {
  detail::low_level_fmt(os, d);
  if (!d.ok())
    os << " is not a valid Day";
  return os;
}

RD_TIME_NAMESPACE_END
