/*
* Created by boil on 2022/9/17.
*/

#ifndef RENDU_ENUM_H_
#define RENDU_ENUM_H_

#include "enum_detail.h"

namespace rendu {

// Checks is rendu supported compiler.
  inline constexpr bool is_magic_enum_supported = detail::supported<void>::value;

  template<typename T>
  using Enum = detail::enum_concept<T>;

  template<typename T>
  using underlying_type_t = typename detail::underlying_type<T>::type;

  template<auto V>
  using enum_constant = detail::enum_constant<V>;

// Returns type name of enum.
  template<typename E>
  [[nodiscard]] constexpr auto enum_type_name() noexcept -> detail::enable_if_t<E, string_view> {
    constexpr string_view name = detail::type_name_v<std::decay_t<E>>;
    static_assert(!name.empty(), "rendu::enum_type_name enum type does not have a name.");

    return name;
  }

// Returns number of enum values.
  template<typename E>
  [[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_t<E, std::size_t> {
    return detail::count_v<std::decay_t<E>>;
  }

// Returns enum value at specified index.
// No bounds checking is performed: the behavior is undefined if index >= number of enum values.
  template<typename E>
  [[nodiscard]] constexpr auto enum_value(std::size_t index) noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
    using D = std::decay_t<E>;

    if constexpr (detail::is_sparse_v<D>) {
      return assert((index < detail::count_v<D>)), detail::values_v<D>[index];
    } else {
      constexpr bool is_flag = detail::is_flags_v<D>;
      constexpr auto min = is_flag ? detail::log2(detail::min_v<D>) : detail::min_v<D>;

      return assert((index < detail::count_v<D>)), detail::value<D, min, is_flag>(index);
    }
  }

// Returns enum value at specified index.
  template<typename E, std::size_t I>
  [[nodiscard]] constexpr auto enum_value() noexcept -> detail::enable_if_t<E, std::decay_t<E>> {
    using D = std::decay_t<E>;
    static_assert(I < detail::count_v<D>, "rendu::enum_value out of range.");

    return enum_value<D>(I);
  }

// Returns std::array with enum values, sorted by enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_values() noexcept -> detail::enable_if_t<E, detail::values_t<E>> {
    return detail::values_v<std::decay_t<E>>;
  }

// Returns integer value from enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_integer(E value) noexcept -> detail::enable_if_t<E, underlying_type_t<E>> {
    return static_cast<underlying_type_t<E>>(value);
  }

// Returns underlying value from enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_underlying(E value) noexcept -> detail::enable_if_t<E, underlying_type_t<E>> {
    return static_cast<underlying_type_t<E>>(value);
  }

// Obtains index in enum values from enum value.
// Returns optional with index.
  template<typename E>
  [[nodiscard]] constexpr auto enum_index(E value) noexcept -> detail::enable_if_t<E, optional<std::size_t>> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (detail::count_v<D> == 0) {
      return {}; // Empty enum.
    } else if constexpr (detail::is_sparse_v<D> || detail::is_flags_v<D>) {
#if defined(RENDU_ENUM_NO_HASH)
      for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
        if (enum_value<D>(i) == value) {
          return i;
        }
      }
      return {}; // Invalid value or out of range.
#else
      return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::index>(
          [](std::size_t i) { return optional<std::size_t>{i}; },
          value,
          detail::default_result_type_lambda<optional<std::size_t>>);
#endif
    } else {
      const auto v = static_cast<U>(value);
      if (v >= detail::min_v<D> && v <= detail::max_v<D>) {
        return static_cast<std::size_t>(v - detail::min_v<D>);
      }
      return {}; // Invalid value or out of range.
    }
  }

// Obtains index in enum values from static storage enum variable.
  template<auto V>
  [[nodiscard]] constexpr auto enum_index() noexcept -> detail::enable_if_t<decltype(V), std::size_t> {
    constexpr auto index = enum_index<std::decay_t<decltype(V)>>(V);
    static_assert(index, "rendu::enum_index enum value does not have a index.");

    return *index;
  }

// Returns name from static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
  template<auto V>
  [[nodiscard]] constexpr auto enum_name() noexcept -> detail::enable_if_t<decltype(V), string_view> {
    constexpr string_view name = detail::enum_name_v<std::decay_t<decltype(V)>, V>;
    static_assert(!name.empty(), "rendu::enum_name enum value does not have a name.");

    return name;
  }

// Returns name from enum value.
// If enum value does not have name or value out of range, returns empty string.
  template<typename E>
  [[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_t<E, string_view> {
    using D = std::decay_t<E>;

    if (const auto i = enum_index<D>(value)) {
      return detail::names_v<D>[*i];
    }
    return {};
  }

// Returns name from enum-flags value.
// If enum-flags value does not have name or value out of range, returns empty string.
  template<typename E>
  [[nodiscard]] auto enum_flags_name(E value) -> detail::enable_if_t<E, string> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (detail::is_flags_v<D>) {
      string name;
      auto check_value = U{0};
      for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
        if (const auto v = static_cast<U>(enum_value<D>(i)); (static_cast<U>(value) & v) != 0) {
          check_value |= v;
          const auto n = detail::names_v<D>[i];
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

// Returns std::array with names, sorted by enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_names() noexcept -> detail::enable_if_t<E, detail::names_t<E>> {
    return detail::names_v<std::decay_t<E>>;
  }

// Returns std::array with pairs (value, name), sorted by enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_entries() noexcept -> detail::enable_if_t<E, detail::entries_t<E>> {
    return detail::entries_v<std::decay_t<E>>;
  }

// Allows you to write rendu::enum_cast<foo>("bar", rendu::case_insensitive);
  inline constexpr auto case_insensitive = detail::case_insensitive{};

// Obtains enum value from integer value.
// Returns optional with enum value.
  template<typename E>
  [[nodiscard]] constexpr auto
  enum_cast(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, optional<std::decay_t<E>>> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (detail::count_v<D> == 0) {
      return {}; // Empty enum.
    } else if constexpr (detail::is_sparse_v<D>) {
      if constexpr (detail::is_flags_v<D>) {
        auto check_value = U{0};
        for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
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
        for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
          if (value == static_cast<U>(enum_value<D>(i))) {
            return static_cast<D>(value);
          }
        }
        return {}; // Invalid value or out of range.
#else
        return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
            [](D v) { return optional<D>{v}; },
            static_cast<D>(value),
            detail::default_result_type_lambda<optional<D>>);
#endif
      }
    } else {
      constexpr auto min = detail::min_v<D>;
      constexpr auto max = detail::is_flags_v<D> ? detail::values_ors<D>() : detail::max_v<D>;

      if (value >= min && value <= max) {
        return static_cast<D>(value);
      }
      return {}; // Invalid value or out of range.
    }
  }

// Obtains enum value from name.
// Returns optional with enum value.
  template<typename E, typename BinaryPredicate = std::equal_to<>>
  [[nodiscard]] constexpr auto enum_cast(string_view value,
                                         [[maybe_unused]] BinaryPredicate &&p = {}) noexcept(detail::is_nothrow_invocable<BinaryPredicate>()) -> detail::enable_if_t<E, optional<std::decay_t<E>>, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_cast requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    if constexpr (detail::count_v<D> == 0) {
      return {}; // Empty enum.
    } else if constexpr (detail::is_flags_v<D>) {
      auto result = U{0};
      while (!value.empty()) {
        const auto d = detail::find(value, '|');
        const auto s = (d == string_view::npos) ? value : value.substr(0, d);
        auto f = U{0};
        for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
          if (detail::cmp_equal(s, detail::names_v<D>[i], p)) {
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
    } else if constexpr (detail::count_v<D> > 0) {
      if constexpr (detail::is_default_predicate<BinaryPredicate>()) {
#if defined(RENDU_ENUM_NO_HASH)
        for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
          if (detail::cmp_equal(value, detail::names_v<D>[i], p)) {
            return enum_value<D>(i);
          }
        }
        return {}; // Invalid value or out of range.
#else
        return detail::constexpr_switch<&detail::names_v<D>, detail::case_call_t::index>(
            [](std::size_t i) { return optional<D>{detail::values_v<D>[i]}; },
            value,
            detail::default_result_type_lambda<optional<D>>,
            [&p](string_view lhs, string_view rhs) { return detail::cmp_equal(lhs, rhs, p); });
#endif
      } else {
        for (std::size_t i = 0; i < detail::count_v<D>; ++i) {
          if (detail::cmp_equal(value, detail::names_v<D>[i], p)) {
            return enum_value<D>(i);
          }
        }
        return {}; // Invalid value or out of range.
      }
    }
  }

// Checks whether enum contains enumerator with such enum value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_contains(E value) noexcept -> detail::enable_if_t<E, bool> {
    using D = std::decay_t<E>;
    using U = underlying_type_t<D>;

    return static_cast<bool>(enum_cast<D>(static_cast<U>(value)));
  }

// Checks whether enum contains enumerator with such integer value.
  template<typename E>
  [[nodiscard]] constexpr auto enum_contains(underlying_type_t<E> value) noexcept -> detail::enable_if_t<E, bool> {
    using D = std::decay_t<E>;

    return static_cast<bool>(enum_cast<D>(value));
  }

// Checks whether enum contains enumerator with such name.
  template<typename E, typename BinaryPredicate = std::equal_to<>>
  [[nodiscard]] constexpr auto enum_contains(string_view value,
                                             BinaryPredicate &&p = {}) noexcept(detail::is_nothrow_invocable<BinaryPredicate>()) -> detail::enable_if_t<E, bool, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_contains requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;

    return static_cast<bool>(enum_cast<D>(value, std::forward<BinaryPredicate>(p)));
  }

  template<typename Result = void, typename E, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, E value) -> detail::enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(detail::has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
        std::forward<Lambda>(lambda),
        value,
        detail::default_result_type_lambda<Result>);
  }

  template<typename Result, typename E, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, E value, Result &&result) -> detail::enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(detail::has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    return detail::constexpr_switch<&detail::values_v<D>, detail::case_call_t::value>(
        std::forward<Lambda>(lambda),
        value,
        [&result] { return std::forward<Result>(result); });
  }

  template<typename E, typename Result = void, typename BinaryPredicate = std::equal_to<>, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, string_view name,
                             BinaryPredicate &&p = {}) -> detail::enable_if_t<E, Result, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_switch requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;
    static_assert(detail::has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(name, std::forward<BinaryPredicate>(p))) {
      return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v);
    }
    return detail::default_result_type_lambda<Result>();
  }

  template<typename E, typename Result, typename BinaryPredicate = std::equal_to<>, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, string_view name, Result &&result,
                             BinaryPredicate &&p = {}) -> detail::enable_if_t<E, Result, BinaryPredicate> {
    static_assert(std::is_invocable_r_v<bool, BinaryPredicate, char, char>,
                  "rendu::enum_switch requires bool(char, char) invocable predicate.");
    using D = std::decay_t<E>;
    static_assert(detail::has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(name, std::forward<BinaryPredicate>(p))) {
      return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v, std::forward<Result>(result));
    }
    return std::forward<Result>(result);
  }

  template<typename E, typename Result = void, typename Lambda>
  constexpr auto enum_switch(Lambda &&lambda, underlying_type_t<E> value) -> detail::enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(detail::has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(value)) {
      return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v);
    }
    return detail::default_result_type_lambda<Result>();
  }

  template<typename E, typename Result, typename Lambda>
  constexpr auto
  enum_switch(Lambda &&lambda, underlying_type_t<E> value, Result &&result) -> detail::enable_if_t<E, Result> {
    using D = std::decay_t<E>;
    static_assert(detail::has_hash<D>, "rendu::enum_switch requires no defined RENDU_ENUM_NO_HASH");

    if (const auto v = enum_cast<D>(value)) {
      return enum_switch<Result, D>(std::forward<Lambda>(lambda), *v, std::forward<Result>(result));
    }
    return std::forward<Result>(result);
  }

  template<typename E, typename Lambda, detail::enable_if_t<E, int> = 0>
  constexpr auto enum_for_each(Lambda &&lambda) {
    using D = std::decay_t<E>;
    static_assert(std::is_enum_v<D>, "rendu::enum_for_each requires enum type.");
    constexpr auto sep = std::make_index_sequence<detail::count_v<D>>{};

    if constexpr (detail::all_invocable<D, Lambda>(sep)) {
      return detail::for_each<D>(std::forward<Lambda>(lambda), sep);
    } else {
      static_assert(detail::always_false_v<D>, "rendu::enum_for_each requires invocable of all enum value.");
    }
  }

  template<typename T>
  using is_scoped_enum = detail::is_scoped_enum<T>;
  template<typename T>
  using is_unscoped_enum = detail::is_unscoped_enum<T>;

} // namespace rendu

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // RENDU_ENUM_H_
