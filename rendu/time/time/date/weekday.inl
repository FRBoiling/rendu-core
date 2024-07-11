/*
* Created by boil on 2024/7/12.
*/

#include "weekday.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline unsigned char Weekday::weekday_from_days(int z) noexcept {
  auto u = static_cast<unsigned>(z);
  return static_cast<unsigned char>(z >= -4 ? (u + 4) % 7 : u % 7);
}

constexpr inline Weekday::Weekday(unsigned wd) noexcept
    : wd_(static_cast<decltype(wd_)>(wd != 7 ? wd : 0)) {}

constexpr inline Weekday::Weekday(const detail::SysDays &dp) noexcept
    : wd_(weekday_from_days(dp.time_since_epoch().count())) {}

constexpr inline Weekday::Weekday(const detail::LocalDays &dp) noexcept
    : wd_(weekday_from_days(dp.time_since_epoch().count())) {}

constexpr inline Weekday &Weekday::operator++() noexcept {
  *this += detail::Days{1};
  return *this;
}
constexpr inline Weekday Weekday::operator++(int) noexcept {
  auto tmp(*this);
  ++(*this);
  return tmp;
}
constexpr inline Weekday &Weekday::operator--() noexcept {
  *this -= detail::Days{1};
  return *this;
}
constexpr inline Weekday Weekday::operator--(int) noexcept {
  auto tmp(*this);
  --(*this);
  return tmp;
}

constexpr inline Weekday &Weekday::operator+=(const detail::Days &d) noexcept {
  *this = *this + d;
  return *this;
}

constexpr inline Weekday &Weekday::operator-=(const detail::Days &d) noexcept {
  *this = *this - d;
  return *this;
}

constexpr inline bool Weekday::ok() const noexcept { return wd_ <= 6; }

constexpr inline unsigned Weekday::c_encoding() const noexcept {
  return unsigned{wd_};
}

constexpr inline unsigned Weekday::iso_encoding() const noexcept {
  return unsigned{((wd_ == 0u) ? 7u : wd_)};
}

constexpr inline bool
operator==(const Weekday &x, const Weekday &y) noexcept {
  return x.wd_ == y.wd_;
}

constexpr inline bool operator!=(const Weekday &x, const Weekday &y) noexcept {
  return !(x == y);
}

constexpr inline detail::Days operator-(const Weekday &x, const Weekday &y) noexcept {
  auto const wdu = x.wd_ - y.wd_;
  auto const wk = (wdu >= 0 ? wdu : wdu - 6) / 7;
  return detail::Days{wdu - wk * 7};
}

constexpr inline Weekday operator+(const Weekday &x, const detail::Days &y) noexcept {
  auto const wdu = static_cast<long long>(static_cast<unsigned>(x.wd_)) + y.count();
  auto const wk = (wdu >= 0 ? wdu : wdu - 6) / 7;
  return Weekday{static_cast<unsigned>(wdu - wk * 7)};
}

constexpr inline Weekday operator+(const detail::Days &x, const Weekday &y) noexcept {
  return y + x;
}

constexpr inline Weekday operator-(const Weekday &x, const detail::Days &y) noexcept {
  return x + -y;
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const Weekday &wd) {
  if (wd.ok()) {
    CharT fmt[] = {'%', 'a', 0};
    os << format(fmt, wd);
  } else
    os << wd.c_encoding();
  return os;
}
RD_DETAIL_NAMESPACE_END


template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const Weekday &wd) {
  detail::low_level_fmt(os, wd);
  if (!wd.ok())
    os << " is not a valid Weekday";
  return os;
}

constexpr const Weekday sun{0u};
constexpr const Weekday mon{1u};
constexpr const Weekday tue{2u};
constexpr const Weekday wed{3u};
constexpr const Weekday thu{4u};
constexpr const Weekday fri{5u};
constexpr const Weekday sat{6u};

constexpr const Weekday Monday{1};
constexpr const Weekday Tuesday{2};
constexpr const Weekday Wednesday{3};
constexpr const Weekday Thursday{4};
constexpr const Weekday Friday{5};
constexpr const Weekday Saturday{6};
constexpr const Weekday Sunday{7};

RD_TIME_NAMESPACE_END