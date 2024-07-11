/*
* Created by boil on 2024/7/12.
*/

#include "year_month.h"

#include "date_operators.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline YearMonth::YearMonth(const Year &y, const Month &m) noexcept
    : y_(y),
      m_(m) {}

constexpr inline Year YearMonth::year() const noexcept { return y_; }
constexpr inline Month YearMonth::month() const noexcept { return m_; }
constexpr inline bool YearMonth::ok() const noexcept { return y_.ok() && m_.ok(); }

template<class>
constexpr inline YearMonth &YearMonth::operator+=(const detail::Months &dm) noexcept {
  *this = *this + dm;
  return *this;
}

template<class>
constexpr inline YearMonth &YearMonth::operator-=(const detail::Months &dm) noexcept {
  *this = *this - dm;
  return *this;
}

constexpr inline YearMonth &YearMonth::operator+=(const detail::Years &dy) noexcept {
  *this = *this + dy;
  return *this;
}

constexpr inline YearMonth &YearMonth::operator-=(const detail::Years &dy) noexcept {
  *this = *this - dy;
  return *this;
}

constexpr inline bool operator==(const YearMonth &x, const YearMonth &y) noexcept {
  return x.year() == y.year() && x.month() == y.month();
}

constexpr inline bool operator!=(const YearMonth &x, const YearMonth &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const YearMonth &x, const YearMonth &y) noexcept {
  return x.year() < y.year() ? true
                             : (x.year() > y.year() ? false
                                                    : (x.month() < y.month()));
}

constexpr inline bool operator>(const YearMonth &x, const YearMonth &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const YearMonth &x, const YearMonth &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const YearMonth &x, const YearMonth &y) noexcept {
  return !(x < y);
}

template<class>
constexpr inline YearMonth operator+(const YearMonth &ym, const detail::Months &dm) noexcept {
  auto dmi = static_cast<int>(static_cast<unsigned>(ym.month())) - 1 + dm.count();
  auto dy = (dmi >= 0 ? dmi : dmi - 11) / 12;
  dmi = dmi - dy * 12 + 1;
  return (ym.year() + detail::Years(dy)) / Month(static_cast<unsigned>(dmi));
}

template<class>
constexpr inline YearMonth operator+(const detail::Months &dm, const YearMonth &ym) noexcept {
  return ym + dm;
}

template<class>
constexpr inline YearMonth operator-(const YearMonth &ym, const detail::Months &dm) noexcept {
  return ym + -dm;
}

constexpr inline detail::Months operator-(const YearMonth &x, const YearMonth &y) noexcept {
  return (x.year() - y.year()) +
         detail::Months(static_cast<unsigned>(x.month()) - static_cast<unsigned>(y.month()));
}

constexpr inline YearMonth operator+(const YearMonth &ym, const detail::Years &dy) noexcept {
  return (ym.year() + dy) / ym.month();
}

constexpr inline YearMonth operator+(const detail::Years &dy, const YearMonth &ym) noexcept {
  return ym + dy;
}

constexpr inline YearMonth operator-(const YearMonth &ym, const detail::Years &dy) noexcept {
  return ym + -dy;
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const YearMonth &ym) {
  low_level_fmt(os, ym.year()) << '/';
  return low_level_fmt(os, ym.month());
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonth &ym) {
  detail::low_level_fmt(os, ym);
  if (!ym.ok())
    os << " is not a valid YearMonth";
  return os;
}


RD_TIME_NAMESPACE_END
