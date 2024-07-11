/*
* Created by boil on 2024/7/11.
*/

#include "month.h"

RD_TIME_NAMESPACE_BEGIN

constexpr inline Month::Month(unsigned m) noexcept
    : m_(static_cast<decltype(m_)>(m)) {
}

constexpr inline Month &Month::operator++() noexcept {
  *this += detail::Months{1};
  return *this;
}
constexpr inline Month Month::operator++(int) noexcept {
  auto tmp(*this);
  ++(*this);
  return tmp;
}
constexpr inline Month &Month::operator--() noexcept {
  *this -= detail::Months{1};
  return *this;
}
constexpr inline Month Month::operator--(int) noexcept {
  auto tmp(*this);
  --(*this);
  return tmp;
}

constexpr inline Month &Month::operator+=(const detail::Months &m) noexcept {
  *this = *this + m;
  return *this;
}

constexpr inline Month &Month::operator-=(const detail::Months &m) noexcept {
  *this = *this - m;
  return *this;
}

constexpr inline Month::operator unsigned() const noexcept { return m_; }
constexpr inline bool Month::ok() const noexcept { return 1 <= m_ && m_ <= 12; }

constexpr inline bool operator==(const Month &x, const Month &y) noexcept {
  return static_cast<unsigned>(x) == static_cast<unsigned>(y);
}

constexpr inline bool operator!=(const Month &x, const Month &y) noexcept {
  return !(x == y);
}

constexpr inline bool operator<(const Month &x, const Month &y) noexcept {
  return static_cast<unsigned>(x) < static_cast<unsigned>(y);
}

constexpr inline bool operator>(const Month &x, const Month &y) noexcept {
  return y < x;
}

constexpr inline bool operator<=(const Month &x, const Month &y) noexcept {
  return !(y < x);
}

constexpr inline bool operator>=(const Month &x, const Month &y) noexcept {
  return !(x < y);
}

constexpr inline detail::Months operator-(const Month &x, const Month &y) noexcept {
  auto const d = static_cast<unsigned>(x) - static_cast<unsigned>(y);
  return detail::Months(d <= 11 ? d : d + 12);
}

constexpr inline Month operator+(const Month &x, const detail::Months &y) noexcept {
  auto const mu = static_cast<long long>(static_cast<unsigned>(x)) + y.count() - 1;
  auto const yr = (mu >= 0 ? mu : mu - 11) / 12;
  return Month{static_cast<unsigned>(mu - yr * 12 + 1)};
}

constexpr inline Month operator+(const detail::Months &x, const Month &y) noexcept {
  return y + x;
}

constexpr inline Month operator-(const Month &x, const detail::Months &y) noexcept {
  return x + -y;
}

RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits> &low_level_fmt(std::basic_ostream<CharT, Traits> &os, const Month &m) {
  if (m.ok()) {
    CharT fmt[] = {'%', 'b', 0};
    os << format(os.getloc(), fmt, m);
  } else
    os << static_cast<unsigned>(m);
  return os;
}

RD_DETAIL_NAMESPACE_END

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &operator<<(std::basic_ostream<CharT, Traits> &os, const Month &m) {
  detail::low_level_fmt(os, m);
  if (!m.ok())
    os << " is not a valid Month";
  return os;
}

constexpr const Month jan{1};
constexpr const Month feb{2};
constexpr const Month mar{3};
constexpr const Month apr{4};
constexpr const Month may{5};
constexpr const Month jun{6};
constexpr const Month jul{7};
constexpr const Month aug{8};
constexpr const Month sep{9};
constexpr const Month oct{10};
constexpr const Month nov{11};
constexpr const Month dec{12};

constexpr const Month January{1};
constexpr const Month February{2};
constexpr const Month March{3};
constexpr const Month April{4};
constexpr const Month May{5};
constexpr const Month June{6};
constexpr const Month July{7};
constexpr const Month August{8};
constexpr const Month September{9};
constexpr const Month October{10};
constexpr const Month November{11};
constexpr const Month December{12};

RD_TIME_NAMESPACE_END
