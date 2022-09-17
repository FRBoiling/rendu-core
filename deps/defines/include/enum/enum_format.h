/*
* Created by boil on 2022/9/17.
*/

#ifndef RENDU_ENUM_FORMAT_H_
#define RENDU_ENUM_FORMAT_H_

#if defined(RENDU_ENUM_USE_STD_FORMAT)
#if !defined(__cpp_lib_format)
#  error "Format is not supported"
#endif

#include "magic_enum.hpp"

#if !defined(RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT)
#  define RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT true
#  define RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE
#endif // RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT

namespace magic_enum::customize {
  // customize enum to enable/disable automatic std::format 
  template <typename E>
  constexpr bool enum_format_enabled() noexcept {
    return RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT;
  }
} // magic_enum::customize

#include <format>

template <typename E>
struct std::formatter<E, std::enable_if_t<std::is_enum_v<E> && magic_enum::customize::enum_format_enabled<E>(), char>> : std::formatter<std::string_view, char> {
  auto format(E e, format_context& ctx) {
    using D = std::decay_t<E>;
    if constexpr (magic_enum::detail::is_flags_v<D>) {
      if (auto name = magic_enum::enum_flags_name<D>(e); !name.empty()) {
        return this->std::formatter<std::string_view, char>::format(std::string_view{name.data(), name.size()}, ctx);
      }
    } else {
      if (auto name = magic_enum::enum_name<D>(e); !name.empty()) {
        return this->std::formatter<std::string_view, char>::format(std::string_view{name.data(), name.size()}, ctx);
      }
    }
    constexpr auto type_name = magic_enum::enum_type_name<E>();
    throw std::format_error("Type of " + std::string{type_name.data(), type_name.size()} + " enum value: " + std::to_string(magic_enum::enum_integer<D>(e)) + " is not exists.");
  }
};

#if defined(RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE)
#  undef RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT
#  undef RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE
#endif // RENDU_ENUM_DEFAULT_ENABLE_ENUM_FORMAT_AUTO_DEFINE
#endif // RENDU_ENUM_USE_STD_FORMAT
#endif // RENDU_ENUM_FORMAT_H_
