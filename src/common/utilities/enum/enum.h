/*
* Created by boil on 2022/9/17.
*/

#ifndef RENDU_ENUM_H__
#define RENDU_ENUM_H__

#include "enum_struct.h"

namespace rendu {

// Enum value must be in range [RENDU_ENUM_RANGE_MIN, RENDU_ENUM_RANGE_MAX]. By default RENDU_ENUM_RANGE_MIN = -128, RENDU_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro RENDU_ENUM_RANGE_MIN and RENDU_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.


  static_assert(RENDU_ENUM_RANGE_MAX > RENDU_ENUM_RANGE_MIN,
                "RENDU_ENUM_RANGE_MAX must be greater than RENDU_ENUM_RANGE_MIN.");
  static_assert((RENDU_ENUM_RANGE_MAX - RENDU_ENUM_RANGE_MIN) < (std::numeric_limits<std::uint16_t>::max)(),
                "RENDU_ENUM_RANGE must be less than UINT16_MAX.");

  template<auto V, typename E = std::decay_t<decltype(V)>, std::enable_if_t<std::is_enum_v<E>, int> = 0>
  using enum_constant = std::integral_constant<E, V>;

  template<typename... T>
  inline constexpr bool always_false_v = false;


  template<std::uint16_t N>
  class static_string {
  public:
    constexpr explicit static_string(string_view str) noexcept: static_string{str,
                                                                              std::make_integer_sequence<std::uint16_t, N>{}} {
      assert(str.size() == N);
    }

    constexpr const char *data() const noexcept { return chars_; }

    constexpr std::uint16_t size() const noexcept { return N; }

    constexpr operator string_view() const noexcept { return {data(), size()}; }

  private:
    template<std::uint16_t... I>
    constexpr
    static_string(string_view str, std::integer_sequence<std::uint16_t, I...>) noexcept : chars_{str[I]..., '\0'} {}

    char chars_[static_cast<std::size_t>(N) + 1];
  };

  template<>
  class static_string<0> {
  public:
    constexpr explicit static_string() = default;

    constexpr explicit static_string(string_view) noexcept {}

    constexpr const char *data() const noexcept { return nullptr; }

    constexpr std::uint16_t size() const noexcept { return 0; }

    constexpr operator string_view() const noexcept { return {}; }
  };

  constexpr string_view pretty_name(string_view name) noexcept {
    for (std::size_t i = name.size(); i > 0; --i) {
      if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
            (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
            (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
            #if defined(RENDU_ENUM_ENABLE_NONASCII)
            (name[i - 1] & 0x80) ||
            #endif
            (name[i - 1] == '_'))) {
        name.remove_prefix(i);
        break;
      }
    }

    if (name.size() > 0 && ((name[0] >= 'a' && name[0] <= 'z') ||
                            (name[0] >= 'A' && name[0] <= 'Z') ||
                            #if defined(RENDU_ENUM_ENABLE_NONASCII)
                            (name[0]) & 0x80) ||
                            #endif
                            (name[0] == '_'))) {
      return name;
    }

    return {}; // Invalid name.
  }

  class case_insensitive {
    static constexpr char to_lower(char c) noexcept {
      return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
    }

  public:
    template<typename L, typename R>
    constexpr auto operator()([[maybe_unused]] L lhs, [[maybe_unused]] R rhs) const noexcept -> std::enable_if_t<
        std::is_same_v<std::decay_t<L>, char> && std::is_same_v<std::decay_t<R>, char>, bool> {
#if defined(RENDU_ENUM_ENABLE_NONASCII)
      static_assert(always_false_v<L, R>, "rendu::case_insensitive not supported Non-ASCII feature.");
      return false;
#else
      return to_lower(lhs) == to_lower(rhs);
#endif
    }
  };

  constexpr std::size_t find(string_view str, char c) noexcept {
#if defined(__clang__) && __clang_major__ < 9 && defined(__GLIBCXX__) || defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
    // https://stackoverflow.com/questions/56484834/constexpr-stdstring-viewfind-last-of-doesnt-work-on-clang-8-with-libstdc
    // https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
      constexpr bool workaround = true;
#else
    constexpr bool workaround = false;
#endif

    if constexpr (workaround) {
      for (std::size_t i = 0; i < str.size(); ++i) {
        if (str[i] == c) {
          return i;
        }
      }

      return string_view::npos;
    } else {
      return str.find(c);
    }
  }

  template<typename T, std::size_t N, std::size_t... I>
  constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N], std::index_sequence<I...>) noexcept {
    return {{a[I]...}};
  }

  template<typename BinaryPredicate>
  constexpr bool is_default_predicate() noexcept {
    return std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<string_view::value_type>> ||
           std::is_same_v<std::decay_t<BinaryPredicate>, std::equal_to<>>;
  }

  template<typename BinaryPredicate>
  constexpr bool is_nothrow_invocable() {
    return is_default_predicate<BinaryPredicate>() ||
           std::is_nothrow_invocable_r_v<bool, BinaryPredicate, char, char>;
  }

  template<typename BinaryPredicate>
  constexpr bool cmp_equal(string_view lhs, string_view rhs,
                           [[maybe_unused]] BinaryPredicate &&p) noexcept(is_nothrow_invocable<BinaryPredicate>()) {
#if defined(_MSC_VER) && _MSC_VER < 1920 && !defined(__clang__)
    // https://developercommunity.visualstudio.com/content/problem/360432/vs20178-regression-c-failed-in-test.html
    // https://developercommunity.visualstudio.com/content/problem/232218/c-constexpr-string-view.html
    constexpr bool workaround = true;
#else
    constexpr bool workaround = false;
#endif

    if constexpr (!is_default_predicate<BinaryPredicate>() || workaround) {
      if (lhs.size() != rhs.size()) {
        return false;
      }

      const auto size = lhs.size();
      for (std::size_t i = 0; i < size; ++i) {
        if (!p(lhs[i], rhs[i])) {
          return false;
        }
      }

      return true;
    } else {
      return lhs == rhs;
    }
  }

  template<typename L, typename R>
  constexpr bool cmp_less(L lhs, R rhs) noexcept {
    static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "rendu::cmp_less requires integral type.");

    if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
      // If same signedness (both signed or both unsigned).
      return lhs < rhs;
    } else if constexpr (std::is_same_v<L, bool>) { // bool special case
      return static_cast<R>(lhs) < rhs;
    } else if constexpr (std::is_same_v<R, bool>) { // bool special case
      return lhs < static_cast<L>(rhs);
    } else if constexpr (std::is_signed_v<R>) {
      // If 'right' is negative, then result is 'false', otherwise cast & compare.
      return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
    } else {
      // If 'left' is negative, then result is 'true', otherwise cast & compare.
      return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
    }
  }

  template<typename I>
  constexpr I log2(I value) noexcept {
    static_assert(std::is_integral_v<I>, "rendu::log2 requires integral type.");

    if constexpr (std::is_same_v<I, bool>) { // bool special case
      return assert(false), value;
    } else {
      auto ret = I{0};
      for (; value > I{1}; value >>= I{1}, ++ret) {}

      return ret;
    }
  }

  template<typename T>
  inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

  template<typename E>
  constexpr auto n() noexcept {
    static_assert(is_enum_v<E>, "rendu::n requires enum type.");

    if constexpr (supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
      constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
      constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
      constexpr auto name = string_view{};
#endif
      return static_string<name.size()>{name};
    } else {
      return static_string<0>{}; // Unsupported compiler or Invalid customize.
    }
  }

  template<typename E>
  constexpr auto type_name() noexcept {
    static_assert(is_enum_v<E>, "rendu::n requires enum type.");
    return n<E>();
  }

  template<typename E>
  inline constexpr auto type_name_v = type_name<E>();

  template<typename E, E V>
  constexpr auto n() noexcept {
    static_assert(is_enum_v<E>, "rendu::n requires enum type.");

    if constexpr (supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
      constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
      constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
      constexpr auto name = string_view{};
#endif
      return static_string<name.size()>{name};
    } else {
      return static_string<0>{}; // Unsupported compiler or Invalid customize.
    }
  }

  template<typename E, E V>
  constexpr auto enum_name() noexcept {
    static_assert(is_enum_v<E>, "rendu::n requires enum type.");
    return n<E, V>();
  }

  template<typename E, E V>
  inline constexpr auto enum_name_v = enum_name<E, V>();

  template<typename E, auto V>
  constexpr bool is_valid() noexcept {
    static_assert(is_enum_v<E>, "rendu::is_valid requires enum type.");

#if defined(__clang__) && __clang_major__ >= 16
    // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
    constexpr E v = __builtin_bit_cast(E, V);
    [[maybe_unused]] constexpr auto custom = enum_name<E>(v);
    static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize_t>, "rendu::customize requires customize_t type.");
    if constexpr (std::is_same_v<std::decay_t<decltype(custom)>, customize_t> && custom.index() == 0) {
      constexpr auto name = std::get<string_view>(custom);
      static_assert(!name.empty(), "rendu::customize requires not empty string.");
      return name.size() != 0;
    } else {
      return n<E, v>().size() != 0;
    }
#else
    return enum_name<E, static_cast<E>(V)>().size() != 0;
#endif
  }

  template<typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
  constexpr U ualue(std::size_t i) noexcept {
    static_assert(is_enum_v<E>, "rendu::ualue requires enum type.");

    if constexpr (std::is_same_v<U, bool>) { // bool special case
      static_assert(O == 0, "rendu::ualue requires valid offset.");

      return static_cast<U>(i);
    } else if constexpr (IsFlags) {
      return static_cast<U>(U{1} << static_cast<U>(static_cast<int>(i) + O));
    } else {
      return static_cast<U>(static_cast<int>(i) + O);
    }
  }

  template<typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
  constexpr E value(std::size_t i) noexcept {
    static_assert(is_enum_v<E>, "rendu::value requires enum type.");

    return static_cast<E>(ualue<E, O, IsFlags>(i));
  }

  template<typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
  constexpr int reflected_min() noexcept {
    static_assert(is_enum_v<E>, "rendu::reflected_min requires enum type.");

    if constexpr (IsFlags) {
      return 0;
    } else {
      constexpr auto lhs = range_min<E>::value;
      constexpr auto rhs = (std::numeric_limits<U>::min)();

      if constexpr (cmp_less(rhs, lhs)) {
        return lhs;
      } else {
        return rhs;
      }
    }
  }

  template<typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
  constexpr int reflected_max() noexcept {
    static_assert(is_enum_v<E>, "rendu::reflected_max requires enum type.");

    if constexpr (IsFlags) {
      return std::numeric_limits<U>::digits - 1;
    } else {
      constexpr auto lhs = range_max<E>::value;
      constexpr auto rhs = (std::numeric_limits<U>::max)();

      if constexpr (cmp_less(lhs, rhs)) {
        return lhs;
      } else {
        return rhs;
      }
    }
  }

  template<typename E, bool IsFlags>
  inline constexpr auto reflected_min_v = reflected_min<E, IsFlags>();

  template<typename E, bool IsFlags>
  inline constexpr auto reflected_max_v = reflected_max<E, IsFlags>();

  template<std::size_t N>
  constexpr std::size_t values_count(const bool (&valid)[N]) noexcept {
    auto count = std::size_t{0};
    for (std::size_t i = 0; i < N; ++i) {
      if (valid[i]) {
        ++count;
      }
    }

    return count;
  }

  template<typename E, bool IsFlags, int Min, std::size_t... I>
  constexpr auto values(std::index_sequence<I...>) noexcept {
    static_assert(is_enum_v<E>, "rendu::values requires enum type.");
    constexpr bool valid[sizeof...(I)] = {is_valid<E, ualue<E, Min, IsFlags>(I)>()...};
    constexpr std::size_t count = values_count(valid);

    if constexpr (count > 0) {
      E values[count] = {};
      for (std::size_t i = 0, v = 0; v < count; ++i) {
        if (valid[i]) {
          values[v++] = value<E, Min, IsFlags>(i);
        }
      }

      return to_array(values, std::make_index_sequence<count>{});
    } else {
      return std::array<E, 0>{};
    }
  }

  template<typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
  constexpr auto values() noexcept {
    static_assert(is_enum_v<E>, "rendu::values requires enum type.");
    constexpr auto min = reflected_min_v<E, IsFlags>;
    constexpr auto max = reflected_max_v<E, IsFlags>;
    constexpr auto range_size = max - min + 1;
    static_assert(range_size > 0, "rendu::enum_range requires valid size.");
    static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(),
                  "rendu::enum_range requires valid size.");

    return values<E, IsFlags, min>(std::make_index_sequence<range_size>{});
  }

  template<typename E, typename U = std::underlying_type_t<E>>
  constexpr bool is_flags_enum() noexcept {
    static_assert(is_enum_v<E>, "rendu::is_flags_enum requires enum type.");

    if constexpr (has_is_flags<E>::value) {
      return enum_range<E>::is_flags;
    } else if constexpr (std::is_same_v<U, bool>) { // bool special case
      return false;
    } else {
#if defined(RENDU_ENUM_NO_CHECK_FLAGS)
      return false;
#else
      constexpr auto flags_values = values<E, true>();
      constexpr auto default_values = values<E, false>();
      if (flags_values.size() == 0 || default_values.size() > flags_values.size()) {
        return false;
      }
      for (std::size_t i = 0; i < default_values.size(); ++i) {
        const auto v = static_cast<U>(default_values[i]);
        if (v != 0 && (v & (v - 1)) != 0) {
          return false;
        }
      }
      return flags_values.size() > 0;
#endif
    }
  }

  template<typename E>
  inline constexpr bool is_flags_v = is_flags_enum<E>();

  template<typename E>
  inline constexpr std::array values_v = values<E, is_flags_v<E>>();

  template<typename E, typename D = std::decay_t<E>>
  using values_t = decltype((values_v<D>));

  template<typename E>
  inline constexpr auto count_v = values_v<E>.size();

  template<typename E, typename U = std::underlying_type_t<E>>
  inline constexpr auto min_v = (count_v<E> > 0) ? static_cast<U>(values_v<E>.front()) : U{0};

  template<typename E, typename U = std::underlying_type_t<E>>
  inline constexpr auto max_v = (count_v<E> > 0) ? static_cast<U>(values_v<E>.back()) : U{0};

  template<typename E, std::size_t... I>
  constexpr auto names(std::index_sequence<I...>) noexcept {
    static_assert(is_enum_v<E>, "rendu::names requires enum type.");

    return std::array<string_view, sizeof...(I)>{{enum_name_v<E, values_v<E>[I]>...}};
  }

  template<typename E>
  inline constexpr std::array names_v = names<E>(std::make_index_sequence<count_v<E>>{});

  template<typename E, typename D = std::decay_t<E>>
  using names_t = decltype((names_v<D>));

  template<typename E, std::size_t... I>
  constexpr auto entries(std::index_sequence<I...>) noexcept {
    static_assert(is_enum_v<E>, "rendu::entries requires enum type.");

    return std::array<std::pair<E, string_view>, sizeof...(I)>{{{values_v<E>[I], enum_name_v<E, values_v<E>[I]>}...}};
  }

  template<typename E>
  inline constexpr std::array entries_v = entries<E>(std::make_index_sequence<count_v<E>>{});

  template<typename E, typename D = std::decay_t<E>>
  using entries_t = decltype((entries_v<D>));

  template<typename E, typename U = std::underlying_type_t<E>>
  constexpr bool is_sparse() noexcept {
    static_assert(is_enum_v<E>, "rendu::is_sparse requires enum type.");

    if constexpr (count_v<E> == 0) {
      return false;
    } else if constexpr (std::is_same_v<U, bool>) { // bool special case
      return false;
    } else {
      constexpr auto max = is_flags_v<E> ? log2(max_v<E>) : max_v<E>;
      constexpr auto min = is_flags_v<E> ? log2(min_v<E>) : min_v<E>;
      constexpr auto range_size = max - min + 1;

      return range_size != count_v<E>;
    }
  }

  template<typename E>
  inline constexpr bool is_sparse_v = is_sparse<E>();

  template<typename E, typename U = std::underlying_type_t<E>>
  constexpr U values_ors() noexcept {
    static_assert(is_enum_v<E>, "rendu::values_ors requires enum type.");

    auto ors = U{0};
    for (std::size_t i = 0; i < count_v<E>; ++i) {
      ors |= static_cast<U>(values_v<E>[i]);
    }

    return ors;
  }


  template<typename T, typename R, typename BinaryPredicate = std::equal_to<>>
  using enable_if_t = typename enable_if_enum<
      std::is_enum_v<std::decay_t<T>> && std::is_invocable_r_v<bool, BinaryPredicate, char, char>, R>::type;

  template<typename T, std::enable_if_t<std::is_enum_v<std::decay_t<T>>, int> = 0>
  using enum_concept = T;

#if defined(RENDU_ENUM_NO_HASH)
  template <typename T>
  inline constexpr bool has_hash = false;
#else
  template<typename T>
  inline constexpr bool has_hash = true;

  template<typename Hash>
  inline constexpr Hash hash_v{};

  template<auto *GlobValues, typename Hash>
  constexpr auto calculate_cases(std::size_t Page) noexcept {
    constexpr std::array values = *GlobValues;
    constexpr std::size_t size = values.size();

    using switch_t = std::invoke_result_t<Hash, typename decltype(values)::value_type>;
    static_assert(std::is_integral_v<switch_t> && !std::is_same_v<switch_t, bool>);
    const std::size_t values_to = (std::min)(static_cast<std::size_t>(256), size - Page);

    std::array<switch_t, 256> result{};
    auto fill = result.begin();
    {
      auto first = values.begin() + static_cast<std::ptrdiff_t>(Page);
      auto last = values.begin() + static_cast<std::ptrdiff_t>(Page + values_to);
      while (first != last) {
        *fill++ = hash_v<Hash>(*first++);
      }
    }

    // dead cases, try to avoid case collisions
    for (switch_t last_value = result[values_to - 1];
         fill != result.end() && last_value != (std::numeric_limits<switch_t>::max)(); *fill++ = ++last_value) {
    }

    {
      auto it = result.begin();
      auto last_value = (std::numeric_limits<switch_t>::min)();
      for (; fill != result.end(); *fill++ = last_value++) {
        while (last_value == *it) {
          ++last_value, ++it;
        }
      }
    }

    return result;
  }

  template<typename R, typename F, typename... Args>
  constexpr R invoke_r(F &&f, Args &&... args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>) {
    if constexpr (std::is_void_v<R>) {
      std::forward<F>(f)(std::forward<Args>(args)...);
    } else {
      return static_cast<R>(std::forward<F>(f)(std::forward<Args>(args)...));
    }
  }

  enum class case_call_t {
    index, value
  };

  template<typename T = void>
  inline constexpr auto default_result_type_lambda = []() noexcept(std::is_nothrow_default_constructible_v<T>) { return T{}; };

  template<>
  inline constexpr auto default_result_type_lambda<void> = []() noexcept {};

  template<auto *Arr, typename Hash>
  constexpr bool no_duplicate() noexcept {
    using value_t = std::decay_t<decltype((*Arr)[0])>;
    using hash_value_t = std::invoke_result_t<Hash, value_t>;
    std::array<hash_value_t, Arr->size()> hashes{};
    std::size_t size = 0;
    for (auto elem: *Arr) {
      hashes[size] = hash_v<Hash>(elem);
      for (auto i = size++; i > 0; --i) {
        if (hashes[i] < hashes[i - 1]) {
          auto tmp = hashes[i];
          hashes[i] = hashes[i - 1];
          hashes[i - 1] = tmp;
        } else if (hashes[i] == hashes[i - 1]) {
          return false;
        } else {
          break;
        }
      }
    }
    return true;
  }

#define RENDU_ENUM_FOR_EACH_256(T) T(0)T(1)T(2)T(3)T(4)T(5)T(6)T(7)T(8)T(9)T(10)T(11)T(12)T(13)T(14)T(15)T(16)T(17)T(18)T(19)T(20)T(21)T(22)T(23)T(24)T(25)T(26)T(27)T(28)T(29)T(30)T(31)          \
  T(32)T(33)T(34)T(35)T(36)T(37)T(38)T(39)T(40)T(41)T(42)T(43)T(44)T(45)T(46)T(47)T(48)T(49)T(50)T(51)T(52)T(53)T(54)T(55)T(56)T(57)T(58)T(59)T(60)T(61)T(62)T(63)                                 \
  T(64)T(65)T(66)T(67)T(68)T(69)T(70)T(71)T(72)T(73)T(74)T(75)T(76)T(77)T(78)T(79)T(80)T(81)T(82)T(83)T(84)T(85)T(86)T(87)T(88)T(89)T(90)T(91)T(92)T(93)T(94)T(95)                                 \
  T(96)T(97)T(98)T(99)T(100)T(101)T(102)T(103)T(104)T(105)T(106)T(107)T(108)T(109)T(110)T(111)T(112)T(113)T(114)T(115)T(116)T(117)T(118)T(119)T(120)T(121)T(122)T(123)T(124)T(125)T(126)T(127)     \
  T(128)T(129)T(130)T(131)T(132)T(133)T(134)T(135)T(136)T(137)T(138)T(139)T(140)T(141)T(142)T(143)T(144)T(145)T(146)T(147)T(148)T(149)T(150)T(151)T(152)T(153)T(154)T(155)T(156)T(157)T(158)T(159) \
  T(160)T(161)T(162)T(163)T(164)T(165)T(166)T(167)T(168)T(169)T(170)T(171)T(172)T(173)T(174)T(175)T(176)T(177)T(178)T(179)T(180)T(181)T(182)T(183)T(184)T(185)T(186)T(187)T(188)T(189)T(190)T(191) \
  T(192)T(193)T(194)T(195)T(196)T(197)T(198)T(199)T(200)T(201)T(202)T(203)T(204)T(205)T(206)T(207)T(208)T(209)T(210)T(211)T(212)T(213)T(214)T(215)T(216)T(217)T(218)T(219)T(220)T(221)T(222)T(223) \
  T(224)T(225)T(226)T(227)T(228)T(229)T(230)T(231)T(232)T(233)T(234)T(235)T(236)T(237)T(238)T(239)T(240)T(241)T(242)T(243)T(244)T(245)T(246)T(247)T(248)T(249)T(250)T(251)T(252)T(253)T(254)T(255)

#define RENDU_ENUM_CASE(val)                                                                                                      \
  case cases[val]:                                                                                                                \
    if constexpr ((val) + Page < size) {                                                                                          \
      if (!pred(values[val + Page], searched)) {                                                                                  \
        break;                                                                                                                    \
      }                                                                                                                           \
      if constexpr (CallValue == case_call_t::index) {                                                                            \
        if constexpr (std::is_invocable_r_v<result_t, Lambda, std::integral_constant<std::size_t, val + Page>>) {                 \
          return invoke_r<result_t>(std::forward<Lambda>(lambda), std::integral_constant<std::size_t, val + Page>{});     \
        } else if constexpr (std::is_invocable_v<Lambda, std::integral_constant<std::size_t, val + Page>>) {                      \
          assert(false && "rendu::constexpr_switch wrong result type.");                                             \
        }                                                                                                                         \
      } else if constexpr (CallValue == case_call_t::value) {                                                                     \
        if constexpr (std::is_invocable_r_v<result_t, Lambda, enum_constant<values[val + Page]>>) {                               \
          return invoke_r<result_t>(std::forward<Lambda>(lambda), enum_constant<values[val + Page]>{});                   \
        } else if constexpr (std::is_invocable_r_v<result_t, Lambda, enum_constant<values[val + Page]>>) {                        \
          assert(false && "rendu::constexpr_switch wrong result type.");                                             \
        }                                                                                                                         \
      }                                                                                                                           \
      break;                                                                                                                      \
    } else [[fallthrough]];

  template<auto *GlobValues,
      case_call_t CallValue,
      std::size_t Page = 0,
      typename Hash = constexpr_hash_t<typename std::decay_t<decltype(*GlobValues)>::value_type>,
      typename Lambda, typename ResultGetterType = decltype(default_result_type_lambda<>),
      typename BinaryPredicate = std::equal_to<>>
  constexpr std::invoke_result_t<ResultGetterType> constexpr_switch(
      Lambda &&lambda,
      typename std::decay_t<decltype(*GlobValues)>::value_type searched,
      ResultGetterType &&def = default_result_type_lambda<>,
      BinaryPredicate &&pred = {}) {
    using result_t = std::invoke_result_t<ResultGetterType>;
    using hash_t = std::conditional_t<no_duplicate<GlobValues, Hash>(), Hash, typename Hash::secondary_hash>;
    constexpr std::array values = *GlobValues;
    constexpr std::size_t size = values.size();
    constexpr std::array cases = calculate_cases<GlobValues, hash_t>(Page);

    switch (hash_v<hash_t>(searched)) {
      RENDU_ENUM_FOR_EACH_256(RENDU_ENUM_CASE)
      default:
        if constexpr (size > 256 + Page) {
          return constexpr_switch<GlobValues, CallValue, Page + 256, Hash>(std::forward<Lambda>(lambda), searched,
                                                                           std::forward<ResultGetterType>(def));
        }
        break;
    }
    return def();
  }

#undef RENDU_ENUM_FOR_EACH_256
#undef RENDU_ENUM_CASE
#endif

  template<typename E, typename Lambda, std::size_t... I>
  constexpr auto for_each(Lambda &&lambda, std::index_sequence<I...>) {
    static_assert(is_enum_v<E>, "rendu::for_each requires enum type.");
    constexpr bool has_void_return = (std::is_void_v<std::invoke_result_t<Lambda, enum_constant<values_v<E>[I]>>> || ...);
    constexpr bool all_same_return = (std::is_same_v<std::invoke_result_t<Lambda, enum_constant<values_v<E>[0]>>, std::invoke_result_t<Lambda, enum_constant<values_v<E>[I]>>> && ...);

    if constexpr (has_void_return) {
      (lambda(enum_constant<values_v<E>[I]>{}), ...);
    } else if constexpr (all_same_return) {
      return std::array{lambda(enum_constant<values_v<E>[I]>{})...};
    } else {
      return std::tuple{lambda(enum_constant<values_v<E>[I]>{})...};
    }
  }

  template<typename E, typename Lambda, std::size_t... I>
  constexpr bool all_invocable(std::index_sequence<I...>) {
    static_assert(is_enum_v<E>, "rendu::all_invocable requires enum type.");

    return (std::is_invocable_v<Lambda, enum_constant<values_v<E>[I]>> && ...);
  }


// Checks is rendu supported compiler.
  inline constexpr bool is_magic_enum_supported = supported<void>::value;

  template<typename T>
  using Enum = enum_concept<T>;

  template<typename T>
  inline constexpr bool is_unscoped_enum_v = is_unscoped_enum<T>::value;

  template<typename T>
  inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

  template<typename T>
  using underlying_type_t = typename underlying_type<T>::type;

// Returns type name of enum.
  template<typename E>
  [[nodiscard]] constexpr auto enum_type_name() noexcept -> enable_if_t<E, string_view> {
    constexpr string_view name = type_name_v<std::decay_t<E>>;
    static_assert(!name.empty(), "rendu::enum_type_name enum type does not have a name.");

    return name;
  }

// Returns number of enum values.
  template<typename E>
  [[nodiscard]] constexpr auto enum_count() noexcept -> enable_if_t<E, std::size_t> {
    return count_v<std::decay_t<E>>;
  }

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
  template<typename E>
  [[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> enable_if_t<E, std::decay_t<E>> {
    using D = std::decay_t<E>;

    if constexpr (is_sparse_v<D>) {
      return assert((index < count_v<D>)), values_v<D>[index];
    } else {
      constexpr bool is_flag = is_flags_v<D>;
      constexpr auto min = is_flag ? log2(min_v<D>) : min_v<D>;

      return assert((index < count_v<D>)), value<D, min, is_flag>(index);
    }
  }

// Returns enum value at specified index.
  template<typename E, std::size_t I>
  [[nodiscard]] constexpr auto enum_value() noexcept -> enable_if_t<E, std::decay_t<E>> {
    using D = std::decay_t<E>;
    static_assert(I < count_v<D>, "rendu::enum_value out of range.");

    return enum_value < D > (I);
  }

// Returns std::array with enum values, sorted by enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_values() noexcept -> enable_if_t<E, values_t<E>> {
    return values_v<std::decay_t<E>>;
  }

// Returns integer value from enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_integer(E value) noexcept -> enable_if_t<E, underlying_type_t<E>> {
    return static_cast<underlying_type_t<E>>(value);
  }

// Returns underlying value from enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_underlying(E value) noexcept -> enable_if_t<E, underlying_type_t<E>> {
    return static_cast<underlying_type_t<E>>(value);
  }

// Obtains index in enum values from enum value.
// Returns optional with index.
  template<typename E>
  [[nodiscard]] constexpr auto enum_index(E value) noexcept -> enable_if_t<E, optional<std::size_t>> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (count_v<D> == 0) {
      return {}; // Empty enum.
    } else if constexpr (is_sparse_v<D> || is_flags_v<D>) {
#if defined(RENDU_ENUM_NO_HASH)
      for (std::size_t i = 0; i < count_v<D>; ++i) {
        if (enum_value<D>(i) == value) {
          return i;
        }
      }
      return {}; // Invalid value or out of range.
#else
      return constexpr_switch<&values_v<D>, case_call_t::index>(
          [](std::size_t i) { return optional<std::size_t>{i}; },
          value,
          default_result_type_lambda<optional<std::size_t>>);
#endif
    } else {
      const auto v = static_cast<U>(value);
      if (v >= min_v<D> && v <= max_v<D>) {
        return static_cast<std::size_t>(v - min_v<D> );
      }
      return {}; // Invalid value or out of range.
    }
  }

// Obtains index in enum values from static storage enum variable.
  template<auto V>
  [[nodiscard]] constexpr auto enum_index() noexcept -> enable_if_t<decltype(V), std::size_t> {
    constexpr auto index = enum_index < std::decay_t<decltype(V)>>
    (V);
    static_assert(index, "rendu::enum_index enum value does not have a index.");

    return *index;
  }

  template<auto V>
  constexpr auto enum_name() noexcept -> enable_if_t<decltype(V), string_view> {
    constexpr string_view name = enum_name_v<std::decay_t<decltype(V)>, V>;
    static_assert(!name.empty(), "rendu::enum_name enum value does not have a name.");

    return name;
  }

  template<typename E>
  constexpr auto enum_name(E value) noexcept -> enable_if_t<E, string_view> {
    using D = std::decay_t<E>;

    if (const auto i = enum_index<D>(value)) {
      return names_v<D>[*i];
    }
    return {};
  }

  template<typename E>
  [[nodiscard]] auto enum_flags_name(E value) -> enable_if_t<E, string> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (is_flags_v<D>) {
      string name;
      auto check_value = U{0};
      for (std::size_t i = 0; i < count_v<D>; ++i) {
        if (const auto v = static_cast<U>(enum_value<D>(i)); (static_cast<U>(value) & v) != 0) {
          check_value |= v;
          const auto n = names_v<D>[i];
          if (!name.empty()) {
            name.append(1, '|');
          }
          name.append(n.data(), n.size());
        }
      }

      if (check_value != 0 && check_value == static_cast<U>(value)) {
        return name;
      }

      return {}; // Invalid value or out of range.
    } else {
      if (const auto name = enum_name<D>(value); !name.empty()) {
        return {name.data(), name.size()};
      }
      return {}; // Invalid value or out of range.
    }
  }

  template<typename E>
  [[nodiscard]] constexpr auto enum_names() noexcept -> enable_if_t<E, names_t<E>> {
    return names_v<std::decay_t<E>>;
  }

  template<typename E>
  [[nodiscard]] constexpr auto enum_entries() noexcept -> enable_if_t<E, entries_t<E>> {
    return entries_v<std::decay_t<E>>;
  }

  template<typename E>
  [[nodiscard]] constexpr auto
  enum_cast(underlying_type_t<E> value) noexcept -> enable_if_t<E, optional<std::decay_t<E>>> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (count_v<D> == 0) {
      return {}; // Empty enum.
    } else if constexpr (is_sparse_v<D>) {
      if constexpr (is_flags_v<D>) {
        auto check_value = U{0};
        for (std::size_t i = 0; i < count_v<D>; ++i) {
          if (const auto v = static_cast<U>(enum_value<D>(i)); (value & v) != 0) {
            check_value |= v;
          }
        }

        if (check_value != 0 && check_value == value) {
          return static_cast<D>(value);
        }
        return {}; // Invalid value or out of range.
      } else {
#if defined(RENDU_ENUM_NO_HASH)
        for (std::size_t i = 0; i < count_v<D>; ++i) {
          if (value == static_cast<U>(enum_value<D>(i))) {
            return static_cast<D>(value);
          }
        }
        return {}; // Invalid value or out of range.
#else
        return constexpr_switch<&values_v<D>, case_call_t::value>(
            [](D v) { return optional<D>{v}; },
            static_cast<D>(value),
            default_result_type_lambda<optional<D>>);
#endif
      }
    } else {
      constexpr auto min = min_v<D>;
      constexpr auto max = is_flags_v<D> ? values_ors<D>() : max_v<D>;

      if (value >= min && value <= max) {
        return static_cast<D>(value);
      }
      return {}; // Invalid value or out of range.
    }
  }

  template<typename E, typename BinaryPredicate = std::equal_to<>>
  [[nodiscard]] constexpr auto enum_cast(string_view value,
                                         [[maybe_unused]] BinaryPredicate &&p = {}) noexcept(is_nothrow_invocable<BinaryPredicate>()) -> enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_cast requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (count_v<D> == 0) {
      return {}; // Empty enum.
    } else if constexpr (is_flags_v<D>) {
      auto result = U{0};
      while (!value.empty()) {
        const auto d = find(value, '|');
        const auto s = (d == string_view::npos) ? value : value.substr(0, d);
        auto f = U{0};
        for (std::size_t i = 0; i < count_v<D>; ++i) {
          if (cmp_equal(s, names_v<D>[i], p)) {
            f = static_cast<U>(enum_value<D>(i));
            result |= f;
            break;
          }
        }
        if (f == U{0}) {
          return {}; // Invalid value or out of range.
        }
        value.remove_prefix((d == string_view::npos) ? value.size() : d + 1);
      }

      if (result != U{0}) {
        return static_cast<D>(result);
      }
      return {}; // Invalid value or out of range.
    } else if constexpr (count_v<D> > 0) {
      if constexpr (is_default_predicate<BinaryPredicate>()) {
#if defined(RENDU_ENUM_NO_HASH)
        for (std::size_t i = 0; i < count_v<D>; ++i) {
          if (cmp_equal(value, names_v<D>[i], p)) {
            return enum_value<D>(i);
          }
        }
        return {}; // Invalid value or out of range.
#else
        return constexpr_switch<&names_v<D>, case_call_t::index>(
            [](std::size_t i) { return optional<D>{values_v<D>[i]}; },
            value,
            default_result_type_lambda<optional<D>>,
            [&p](string_view lhs, string_view rhs) { return cmp_equal(lhs, rhs, p); });
#endif
      } else {
        for (std::size_t i = 0; i < count_v<D>; ++i) {
          if (cmp_equal(value, names_v<D>[i], p)) {
            return enum_value<D>(i);
          }
        }
        return {}; // Invalid value or out of range.
      }
    }
  }

  template<typename E>
  [[nodiscard]] constexpr auto enum_contains(E value) noexcept -> enable_if_t<E, bool> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    return static_cast<bool>(enum_cast<D>(static_cast<U>(value)));
  }

  template<typename E>
  [[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> enable_if_t<E, bool> {
    using D = std::decay_t<E>;

    return static_cast<bool>(enum_cast<D>(value));
  }

  template<typename E, typename BinaryPredicate = std::equal_to<>>
  [[nodiscard]] constexpr auto enum_contains(string_view value,
                                             BinaryPredicate &&p = {}) noexcept(is_nothrow_invocable<BinaryPredicate>()) -> enable_if_t<E, bool, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_contains requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;

    return static_cast<bool>(enum_cast<D>(value, std::forward<BinaryPredicate>(p)));
  }

  template<typename Result = void, typename E, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, E value) -> enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    return constexpr_switch<&values_v<D>, case_call_t::value>(
        std::forward<Lambda>(lambda),
        value,
        default_result_type_lambda<Result>);
  }

  template<typename Result, typename E, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, E value, Result &&result) -> enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    return constexpr_switch<&values_v<D>, case_call_t::value>(
        std::forward<Lambda>(lambda),
        value,
        [&result] { return std::forward<Result>(result); });
  }

  template<typename E, typename Result = void, typename BinaryPredicate = std::equal_to<>, typename Lambda>
  constexpr auto
  enum_switch(Lambda &&lambda, string_view name, BinaryPredicate &&p = {}) -> enable_if_t<E, Result, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_switch requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;
    static_assert(has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(name, std::forward<BinaryPredicate>(p))) {
      return enum_switch < Result, D > (std::forward<Lambda>(lambda), *v);
    }
    return default_result_type_lambda<Result>();
  }

  template<typename E, typename Result, typename BinaryPredicate = std::equal_to<>, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, string_view name, Result &&result,
                             BinaryPredicate &&p = {}) -> enable_if_t<E, Result, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_switch requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;
    static_assert(has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(name, std::forward<BinaryPredicate>(p))) {
      return enum_switch < Result, D > (std::forward<Lambda>(lambda), *v, std::forward<Result>(result));
    }
    return std::forward<Result>(result);
  }

  template<typename E, typename Result = void, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, underlying_type_t<E> value) -> enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(value)) {
      return enum_switch < Result, D > (std::forward<Lambda>(lambda), *v);
    }
    return default_result_type_lambda<Result>();
  }

  template<typename E, typename Result, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, underlying_type_t<E> value, Result &&result) -> enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(value)) {
      return enum_switch < Result, D > (std::forward<Lambda>(lambda), *v, std::forward<Result>(result));
    }
    return std::forward<Result>(result);
  }

  template<typename E, typename Lambda, enable_if_t<E, int> = 0>
  constexpr auto enum_for_each(Lambda &&lambda) {
    using D = std::decay_t<E>;
    static_assert(std::is_enum_v<D>, "rendu::enum_for_each requires enum type.");
    constexpr auto sep = std::make_index_sequence<count_v<D>>
        {};

    if constexpr (all_invocable<D, Lambda>(sep)) {
      return for_each<D>(std::forward<Lambda>(lambda), sep);
    } else {
      static_assert(always_false_v<D>, "rendu::enum_for_each requires invocable of all enum value.");
    }
  }


  template<typename Char, typename Traits, typename E, enable_if_t<E, int> = 0>
  std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os, E value) {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (supported<D>::value) {
      if (const auto name = enum_flags_name<D>(value); !name.empty()) {
        for (const auto c: name) {
          os.put(c);
        }
        return os;
      }
    }
    return (os << static_cast<U>(value));
  }

  template<typename Char, typename Traits, typename E, enable_if_t<E, int> = 0>
  std::basic_ostream<Char, Traits> &operator<<(std::basic_ostream<Char, Traits> &os, optional<E> value) {
    return value ? (os << *value) : os;
  }

  template<typename Char, typename Traits, typename E, enable_if_t<E, int> = 0>
  std::basic_istream<Char, Traits> &operator>>(std::basic_istream<Char, Traits> &is, E &value) {
    using D = std::decay_t<E>;

    std::basic_string<Char, Traits> s;
    is >> s;
    if (const auto v = enum_cast<D>(s)) {
      value = *v;
    } else {
      is.setstate(std::basic_ios<Char>::failbit);
    }
    return is;
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E operator~(E rhs) noexcept {
    return static_cast<E>(~static_cast<underlying_type_t<E>>(rhs));
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E operator|(E lhs, E rhs) noexcept {
    return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) | static_cast<underlying_type_t<E>>(rhs));
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E operator&(E lhs, E rhs) noexcept {
    return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) & static_cast<underlying_type_t<E>>(rhs));
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E operator^(E lhs, E rhs) noexcept {
    return static_cast<E>(static_cast<underlying_type_t<E>>(lhs) ^ static_cast<underlying_type_t<E>>(rhs));
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E &operator|=(E &lhs, E rhs) noexcept {
    return lhs = (lhs | rhs);
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E &operator&=(E &lhs, E rhs) noexcept {
    return lhs = (lhs & rhs);
  }

  template<typename E, enable_if_t<E, int> = 0>
  constexpr E &operator^=(E &lhs, E rhs) noexcept {
    return lhs = (lhs ^ rhs);
  }

  template<typename E>
  constexpr optional<std::uintmax_t> fuse_one_enum(optional<std::uintmax_t> hash, E value) noexcept {
    if (hash) {
      if (const auto index = enum_index(value)) {
        return (*hash << log2(enum_count<E>() + 1)) | *index;
      }
    }
    return {};
  }

  template<typename E>
  constexpr optional<std::uintmax_t> fuse_enum(E value) noexcept {
    return fuse_one_enum(0, value);
  }

  template<typename E, typename... Es>
  constexpr optional<std::uintmax_t> fuse_enum(E head, Es... tail) noexcept {
    return fuse_one_enum(fuse_enum(tail...), head);
  }

  template<typename... Es>
  constexpr auto typesafe_fuse_enum(Es... values) noexcept {
    enum class enum_fuse_t : std::uintmax_t;
    const auto fuse = fuse_enum(values...);
    if (fuse) {
      return optional<enum_fuse_t>{static_cast<enum_fuse_t>(*fuse)};
    }
    return optional<enum_fuse_t>{};
  }

} // namespace rendu



#endif // RENDU_ENUM_H__
