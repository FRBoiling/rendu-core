/*
* Created by boil on 2024/7/12.
*/

#include "weekday_indexed.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline Weekday WeekdayIndexed::weekday() const noexcept {
  return Weekday{static_cast<unsigned>(wd_)};
}

constexpr inline unsigned WeekdayIndexed::index() const noexcept { return index_; }

constexpr inline bool WeekdayIndexed::ok() const noexcept {
  return weekday().ok() && 1 <= index_ && index_ <= 5;
}

constexpr inline WeekdayIndexed::WeekdayIndexed(const Weekday &wd, unsigned index) noexcept
    : wd_(static_cast<decltype(wd_)>(static_cast<unsigned>(wd.getWd()))), index_(static_cast<decltype(index_)>(index)) {}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const WeekdayIndexed &wdi) {
  return low_level_fmt(os, wdi.weekday()) << '[' << wdi.index() << ']';
}
RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const WeekdayIndexed &wdi) {
  detail::low_level_fmt(os, wdi);
  if (!wdi.ok())
    os << " is not a valid WeekdayIndexed";
  return os;
}


constexpr inline bool operator==(const WeekdayIndexed &x, const WeekdayIndexed &y) noexcept {
  return x.weekday() == y.weekday() && x.index() == y.index();
}

constexpr inline bool operator!=(const WeekdayIndexed &x, const WeekdayIndexed &y) noexcept {
  return !(x == y);
}

constexpr inline WeekdayIndexed Weekday::operator[](unsigned index) const noexcept {
  return {*this, index};
}

RD_TIME_NAMESPACE_END
