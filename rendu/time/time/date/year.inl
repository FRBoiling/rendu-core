/*
* Created by boil on 2024/7/11.
*/

#include "year.h"
#include "utilities.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline Year::Year(int y) noexcept
    : y_(static_cast<decltype(y_)>(y)) {
}

constexpr inline Year &Year::operator++() noexcept {
  ++y_;
  return *this;
}
constexpr inline Year Year::operator++(int) noexcept {
  auto tmp(*this);
  ++(*this);
  return tmp;
}
constexpr inline Year &Year::operator--() noexcept {
  --y_;
  return *this;
}
constexpr inline Year Year::operator--(int) noexcept {
  auto tmp(*this);
  --(*this);
  return tmp;
}
constexpr inline Year &Year::operator+=(const detail::Years &y) noexcept {
  *this = *this + y;
  return *this;
}
constexpr inline Year &Year::operator-=(const detail::Years &y) noexcept {
  *this = *this - y;
  return *this;
}
constexpr inline Year Year::operator-() const noexcept { return Year{-y_}; }
constexpr inline Year Year::operator+() const noexcept { return *this; }

constexpr inline bool Year::is_leap() const noexcept {
  return y_ % 4 == 0 && (y_ % 100 != 0 || y_ % 400 == 0);
}

constexpr inline Year::operator int() const noexcept { return y_; }

constexpr inline bool Year::ok() const noexcept {
  return y_ != std::numeric_limits<short>::min();
}

constexpr inline bool operator==(const Year &x, const Year &y) noexcept {
  return static_cast<int>(x) == static_cast<int>(y);
}

constexpr inline bool operator!=(const Year &x, const Year &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const Year &x, const Year &y) noexcept {
  return static_cast<int>(x) < static_cast<int>(y);
}

constexpr inline bool operator>(const Year &x, const Year &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const Year &x, const Year &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const Year &x, const Year &y) noexcept {
  return !(x < y);
}

constexpr inline detail::Years operator-(const Year &x, const Year &y) noexcept {
  return detail::Years{static_cast<int>(x) - static_cast<int>(y)};
}

constexpr inline Year operator+(const Year &x, const detail::Years &y) noexcept {
  return Year{static_cast<int>(x) + y.count()};
}

constexpr inline Year operator+(const detail::Years &x, const Year &y) noexcept {
  return y + x;
}

constexpr inline Year operator-(const Year &x, const detail::Years &y) noexcept {
  return Year{static_cast<int>(x) - y.count()};
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> & low_level_fmt(std::basic_ostream<CharT, Traits> &os, const Year &y) {
  detail::save_ostream<CharT, Traits> _(os);
  os.fill('0');
  os.flags(std::ios::dec | std::ios::internal);
  os.width(4 + (y < Year{0}));
  os.imbue(std::locale::classic());
  os << static_cast<int>(y);
  return os;
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> &os, const Year &y) {
  detail::low_level_fmt(os, y);
  if (!y.ok())
    os << " is not a valid Year";
  return os;
}

RD_TIME_NAMESPACE_END