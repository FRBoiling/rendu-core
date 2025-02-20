/*
* Created by boil on 2024/7/11.
*/

#ifndef RENDU_TIME_TIME_DETAIL_DETAIL_UTILS_H_
#define RENDU_TIME_TIME_DETAIL_DETAIL_UTILS_H_

#include "time_define.h"
// utilities
RD_TIME_NAMESPACE_BEGIN
RD_DETAIL_NAMESPACE_BEGIN

template<class CharT, class Traits = std::char_traits<CharT>>
class save_istream {
protected:
  std::basic_ios<CharT, Traits> &is_;
  CharT fill_;
  std::ios::fmtflags flags_;
  std::streamsize precision_;
  std::streamsize width_;
  std::basic_ostream<CharT, Traits> *tie_;
  std::locale loc_;

public:
  ~save_istream() {
    is_.fill(fill_);
    is_.flags(flags_);
    is_.precision(precision_);
    is_.width(width_);
    is_.imbue(loc_);
    is_.tie(tie_);
  }

  save_istream(const save_istream &) = delete;
  save_istream &operator=(const save_istream &) = delete;

  explicit save_istream(std::basic_ios<CharT, Traits> &is)
      : is_(is), fill_(is.fill()), flags_(is.flags()), precision_(is.precision()), width_(is.width(0)), tie_(is.tie(nullptr)), loc_(is.getloc()) {
    if (tie_ != nullptr)
      tie_->flush();
  }
};

template<class CharT, class Traits = std::char_traits<CharT>>
class save_ostream : private save_istream<CharT, Traits> {
public:
  ~save_ostream() {
    if ((this->flags_ & std::ios::unitbuf) &&
#if HAS_UNCAUGHT_EXCEPTIONS
        std::uncaught_exceptions() == 0 &&
#else
        !std::uncaught_exception() &&
#endif
        this->is_.good())
      this->is_.rdbuf()->pubsync();
  }

  save_ostream(const save_ostream &) = delete;
  save_ostream &operator=(const save_ostream &) = delete;

  explicit save_ostream(std::basic_ios<CharT, Traits> &os)
      : save_istream<CharT, Traits>(os) {
  }
};

template<class T>
struct choose_trunc_type {
  static const int digits = std::numeric_limits<T>::digits;
  using type = typename std::conditional < digits < 32, std::int32_t, typename std::conditional<digits<64, std::int64_t,
#ifdef __SIZEOF_INT128__
                                                                                                       __int128
#else
                                                                                                       std::int64_t
#endif
                                                                                                       >::type>::type;
};

template<class T>
constexpr inline
    typename std::enable_if<!std::chrono::treat_as_floating_point<T>::value, T>::type
    trunc(T t) noexcept {
  return t;
}

template<class T>
constexpr inline
    typename std::enable_if<std::chrono::treat_as_floating_point<T>::value, T>::type
    trunc(T t) noexcept {
  using std::numeric_limits;
  using I = typename choose_trunc_type<T>::type;
  constexpr const auto digits = numeric_limits<T>::digits;
  static_assert(digits < numeric_limits<I>::digits, "");
  constexpr const auto max = I{1} << (digits - 1);
  constexpr const auto min = -max;
  const auto negative = t < T{0};
  if (min <= t && t <= max && t != 0 && t == t) {
    t = static_cast<T>(static_cast<I>(t));
    if (t == 0 && negative)
      t = -t;
  }
  return t;
}

template<std::intmax_t Xp, std::intmax_t Yp>
struct static_gcd {
  static const std::intmax_t value = static_gcd<Yp, Xp % Yp>::value;
};

template<std::intmax_t Xp>
struct static_gcd<Xp, 0> {
  static const std::intmax_t value = Xp;
};

template<>
struct static_gcd<0, 0> {
  static const std::intmax_t value = 1;
};

template<class R1, class R2>
struct no_overflow {
private:
  static const std::intmax_t gcd_n1_n2 = static_gcd<R1::num, R2::num>::value;
  static const std::intmax_t gcd_d1_d2 = static_gcd<R1::den, R2::den>::value;
  static const std::intmax_t n1 = R1::num / gcd_n1_n2;
  static const std::intmax_t d1 = R1::den / gcd_d1_d2;
  static const std::intmax_t n2 = R2::num / gcd_n1_n2;
  static const std::intmax_t d2 = R2::den / gcd_d1_d2;
#ifdef __cpp_constexpr
  static const std::intmax_t max = std::numeric_limits<std::intmax_t>::max();
#else
  static const std::intmax_t max = LLONG_MAX;
#endif

  template<std::intmax_t Xp, std::intmax_t Yp, bool overflow>
  struct mul// overflow == false
  {
    static const std::intmax_t value = Xp * Yp;
  };

  template<std::intmax_t Xp, std::intmax_t Yp>
  struct mul<Xp, Yp, true> {
    static const std::intmax_t value = 1;
  };

public:
  static const bool value = (n1 <= max / d2) && (n2 <= max / d1);
  typedef std::ratio<mul<n1, d2, !value>::value, mul<n2, d1, !value>::value> type;
};


RD_DETAIL_NAMESPACE_END

// trunc towards zero
template<class To, class Rep, class Period>
constexpr inline
    typename std::enable_if<detail::no_overflow<Period, typename To::period>::value, To>::type
    trunc(const std::chrono::duration<Rep, Period> &d) {
  return To{detail::trunc(std::chrono::duration_cast<To>(d).count())};
}

template<class To, class Rep, class Period>
constexpr inline
    typename std::enable_if<!detail::no_overflow<Period, typename To::period>::value, To>::type
    trunc(const std::chrono::duration<Rep, Period> &d) {
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using rep = typename std::common_type<Rep, typename To::rep>::type;
  return To{detail::trunc(duration_cast<To>(duration_cast<duration<rep>>(d)).count())};
}

#ifndef HAS_CHRONO_ROUNDING
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 190023918 || (_MSC_FULL_VER >= 190000000 && defined(__clang__)))
#define HAS_CHRONO_ROUNDING 1
#elif defined(__cpp_lib_chrono) && __cplusplus > 201402 && __cpp_lib_chrono >= 201510
#define HAS_CHRONO_ROUNDING 1
#elif defined(_LIBCPP_VERSION) && __cplusplus > 201402 && _LIBCPP_VERSION >= 3800
#define HAS_CHRONO_ROUNDING 1
#else
#define HAS_CHRONO_ROUNDING 0
#endif
#endif// HAS_CHRONO_ROUNDING

#if HAS_CHRONO_ROUNDING == 0

// round down
template<class To, class Rep, class Period>
constexpr inline
    typename std::enable_if<
        detail::no_overflow<Period, typename To::period>::value,
        To>::type
    floor(const std::chrono::duration<Rep, Period> &d) {
  auto t = trunc<To>(d);
  if (t > d)
    return t - To{1};
  return t;
}

template<class To, class Rep, class Period>
constexpr inline
    typename std::enable_if<
        !detail::no_overflow<Period, typename To::period>::value,
        To>::type
    floor(const std::chrono::duration<Rep, Period> &d) {
  using rep = typename std::common_type<Rep, typename To::rep>::type;
  return floor<To>(floor<std::chrono::duration<rep>>(d));
}

// round to nearest, to even on tie
template<class To, class Rep, class Period>
constexpr inline To
round(const std::chrono::duration<Rep, Period> &d) {
  auto t0 = floor<To>(d);
  auto t1 = t0 + To{1};
  if (t1 == To{0} && t0 < To{0})
    t1 = -t1;
  auto diff0 = d - t0;
  auto diff1 = t1 - d;
  if (diff0 == diff1) {
    if (t0 - trunc<To>(t0 / 2) * 2 == To{0})
      return t0;
    return t1;
  }
  if (diff0 < diff1)
    return t0;
  return t1;
}

// round up
template<class To, class Rep, class Period>
constexpr inline To
ceil(const std::chrono::duration<Rep, Period> &d) {
  auto t = trunc<To>(d);
  if (t < d)
    return t + To{1};
  return t;
}

template<class Rep, class Period,
         class = typename std::enable_if<
             std::numeric_limits<Rep>::is_signed>::type>
constexpr std::chrono::duration<Rep, Period>
abs(std::chrono::duration<Rep, Period> d) {
  return d >= d.zero() ? d : static_cast<decltype(d)>(-d);
}

// round down
template<class To, class Clock, class FromDuration>
constexpr inline std::chrono::time_point<Clock, To>
floor(const std::chrono::time_point<Clock, FromDuration> &tp) {
  using std::chrono::time_point;
  return time_point<Clock, To>{date::floor<To>(tp.time_since_epoch())};
}

// round to nearest, to even on tie
template<class To, class Clock, class FromDuration>
constexpr inline std::chrono::time_point<Clock, To>
round(const std::chrono::time_point<Clock, FromDuration> &tp) {
  using std::chrono::time_point;
  return time_point<Clock, To>{round<To>(tp.time_since_epoch())};
}

// round up
template<class To, class Clock, class FromDuration>
constexpr inline std::chrono::time_point<Clock, To>
ceil(const std::chrono::time_point<Clock, FromDuration> &tp) {
  using std::chrono::time_point;
  return time_point<Clock, To>{ceil<To>(tp.time_since_epoch())};
}

#else// HAS_CHRONO_ROUNDING == 1

using std::chrono::abs;
using std::chrono::ceil;
using std::chrono::floor;
using std::chrono::round;

#endif// HAS_CHRONO_ROUNDING

RD_DETAIL_NAMESPACE_BEGIN

template<class To, class Rep, class Period>
constexpr inline
    typename std::enable_if<!std::chrono::treat_as_floating_point<typename To::rep>::value, To>::type
    round_i(const std::chrono::duration<Rep, Period> &d) {
  return round<To>(d);
}

template<class To, class Rep, class Period>
constexpr inline
    typename std::enable_if<std::chrono::treat_as_floating_point<typename To::rep>::value, To>::type
    round_i(const std::chrono::duration<Rep, Period> &d) {
  return d;
}

template<class To, class Clock, class FromDuration>
constexpr inline
    std::chrono::time_point<Clock, To> round_i(const std::chrono::time_point<Clock, FromDuration> &tp) {
  using std::chrono::time_point;
  return time_point<Clock, To>{round_i<To>(tp.time_since_epoch())};
}

RD_DETAIL_NAMESPACE_END

// trunc towards zero
template<class To, class Clock, class FromDuration>
constexpr inline
    std::chrono::time_point<Clock, To> trunc(const std::chrono::time_point<Clock, FromDuration> &tp) {
  using std::chrono::time_point;
  return time_point<Clock, To>{trunc<To>(tp.time_since_epoch())};
}


RD_TIME_NAMESPACE_END
#endif//RENDU_TIME_TIME_DETAIL_DETAIL_UTILS_H_
