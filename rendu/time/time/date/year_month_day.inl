/*
* Created by boil on 2024/7/12.
*/

#include "year_month_day.h"
#include "year_month_day_last.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline YearMonthDay::YearMonthDay(const Year &y, const Month &m, const Day &d) noexcept
    : y_(y), m_(m), d_(d) {}

constexpr inline YearMonthDay::YearMonthDay(const YearMonthDayLast &ymdl) noexcept
    : y_(ymdl.year()), m_(ymdl.month()), d_(ymdl.day()) {}

constexpr inline YearMonthDay::YearMonthDay(detail::SysDays dp) noexcept
    : YearMonthDay(from_days(dp.time_since_epoch())) {}

constexpr inline YearMonthDay::YearMonthDay(detail::LocalDays dp) noexcept
    : YearMonthDay(from_days(dp.time_since_epoch())) {}

constexpr inline Year YearMonthDay::year() const noexcept { return y_; }
constexpr inline Month YearMonthDay::month() const noexcept { return m_; }
constexpr inline Day YearMonthDay::day() const noexcept { return d_; }

template<class>
constexpr inline YearMonthDay &YearMonthDay::operator+=(const detail::Months &m) noexcept {
  *this = *this + m;
  return *this;
}

template<class>
constexpr inline YearMonthDay &YearMonthDay::operator-=(const detail::Months &m) noexcept {
  *this = *this - m;
  return *this;
}

constexpr inline YearMonthDay &YearMonthDay::operator+=(const detail::Years &y) noexcept {
  *this = *this + y;
  return *this;
}

constexpr inline YearMonthDay &
YearMonthDay::operator-=(const detail::Years &y) noexcept {
  *this = *this - y;
  return *this;
}

constexpr inline detail::Days YearMonthDay::to_days() const noexcept {
  static_assert(std::numeric_limits<unsigned>::digits >= 18,
                "This algorithm has not been ported to a 16 bit unsigned integer");
  static_assert(std::numeric_limits<int>::digits >= 20,
                "This algorithm has not been ported to a 16 bit signed integer");
  auto const y = static_cast<int>(y_) - (m_ <= February);
  auto const m = static_cast<unsigned>(m_);
  auto const d = static_cast<unsigned>(d_);
  auto const era = (y >= 0 ? y : y - 399) / 400;
  auto const yoe = static_cast<unsigned>(y - era * 400);           // [0, 399]
  auto const doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;// [0, 365]
  auto const doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;          // [0, 146096]
  return detail::Days{era * 146097 + static_cast<int>(doe) - 719468};
}

constexpr inline YearMonthDay::operator detail::SysDays() const noexcept {
  return detail::SysDays{to_days()};
}

constexpr inline YearMonthDay::operator detail::LocalDays() const noexcept {
  return detail::LocalDays{to_days()};
}

constexpr inline bool YearMonthDay::ok() const noexcept {
  if (!(y_.ok() && m_.ok()))
    return false;
  return Day{1} <= d_ && d_ <= (y_ / m_ / last).day();
}

constexpr inline bool operator==(const YearMonthDay &x, const YearMonthDay &y) noexcept {
  return x.year() == y.year() && x.month() == y.month() && x.day() == y.day();
}

constexpr inline bool operator!=(const YearMonthDay &x, const YearMonthDay &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const YearMonthDay &x, const YearMonthDay &y) noexcept {
  return x.year() < y.year() ? true
                             : (x.year() > y.year() ? false
                                                    : (x.month() < y.month() ? true
                                                                             : (x.month() > y.month() ? false
                                                                                                      : (x.day() < y.day()))));
}

constexpr inline bool operator>(const YearMonthDay &x, const YearMonthDay &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const YearMonthDay &x, const YearMonthDay &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const YearMonthDay &x, const YearMonthDay &y) noexcept {
  return !(x < y);
}

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const YearMonthDay &ymd) {
  detail::save_ostream<CharT, Traits> _(os);
  os.fill('0');
  os.flags(std::ios::dec | std::ios::right);
  os.imbue(std::locale::classic());
  os << static_cast<int>(ymd.year()) << '-';
  os.width(2);
  os << static_cast<unsigned>(ymd.month()) << '-';
  os.width(2);
  os << static_cast<unsigned>(ymd.day());
  if (!ymd.ok())
    os << " is not a valid YearMonthDay";
  return os;
}

constexpr inline YearMonthDay YearMonthDay::from_days(detail::Days dp) noexcept {
  static_assert(std::numeric_limits<unsigned>::digits >= 18,
                "This algorithm has not been ported to a 16 bit unsigned integer");
  static_assert(std::numeric_limits<int>::digits >= 20,
                "This algorithm has not been ported to a 16 bit signed integer");
  auto const z = dp.count() + 719468;
  auto const era = (z >= 0 ? z : z - 146096) / 146097;
  auto const doe = static_cast<unsigned>(z - era * 146097);              // [0, 146096]
  auto const yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;// [0, 399]
  auto const y = static_cast<detail::Days::rep>(yoe) + era * 400;
  auto const doy = doe - (365 * yoe + yoe / 4 - yoe / 100);// [0, 365]
  auto const mp = (5 * doy + 2) / 153;                     // [0, 11]
  auto const d = doy - (153 * mp + 2) / 5 + 1;             // [1, 31]
  auto const m = mp < 10 ? mp + 3 : mp - 9;                // [1, 12]
  return YearMonthDay{Year{y + (m <= 2)}, Month(m), Day(d)};
}

template<class>
constexpr inline YearMonthDay operator+(const YearMonthDay &ymd, const detail::Months &dm) noexcept {
  return (ymd.year() / ymd.month() + dm) / ymd.day();
}

template<class>
constexpr inline YearMonthDay operator+(const detail::Months &dm, const YearMonthDay &ymd) noexcept {
  return ymd + dm;
}

template<class>
constexpr inline YearMonthDay operator-(const YearMonthDay &ymd, const detail::Months &dm) noexcept {
  return ymd + (-dm);
}

constexpr inline YearMonthDay operator+(const YearMonthDay &ymd, const detail::Years &dy) noexcept {
  return (ymd.year() + dy) / ymd.month() / ymd.day();
}

constexpr inline YearMonthDay operator+(const detail::Years &dy, const YearMonthDay &ymd) noexcept {
  return ymd + dy;
}

constexpr inline YearMonthDay operator-(const YearMonthDay &ymd, const detail::Years &dy) noexcept {
  return ymd + (-dy);
}


RD_TIME_NAMESPACE_END