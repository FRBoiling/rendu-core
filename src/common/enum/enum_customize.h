/*
* Created by boil on 2022/9/25.
*/

#ifndef RENDU_ENUM_CUSTOMIZE_H_
#define RENDU_ENUM_CUSTOMIZE_H_

#include "enum_define.h"

namespace rendu::customize {

// Enum value must be in range [RENDU_ENUM_RANGE_MIN, RENDU_ENUM_RANGE_MAX]. By default RENDU_ENUM_RANGE_MIN = -128, RENDU_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro RENDU_ENUM_RANGE_MIN and RENDU_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
  template <typename E>
  struct enum_range {
    static_assert(std::is_enum_v<E>, "rendu::customize::enum_range requires enum type.");
    static constexpr int min = RENDU_ENUM_RANGE_MIN;
    static constexpr int max = RENDU_ENUM_RANGE_MAX;
    static_assert(max > min, "rendu::customize::enum_range requires max > min.");
  };

  static_assert(RENDU_ENUM_RANGE_MAX > RENDU_ENUM_RANGE_MIN, "RENDU_ENUM_RANGE_MAX must be greater than RENDU_ENUM_RANGE_MIN.");
  static_assert((RENDU_ENUM_RANGE_MAX - RENDU_ENUM_RANGE_MIN) < (std::numeric_limits<std::uint16_t>::max)(), "RENDU_ENUM_RANGE must be less than UINT16_MAX.");

  namespace detail {
    enum class default_customize_tag {};
    enum class invalid_customize_tag {};
  } // namespace rendu::customize::detail

  using customize_t = std::variant<string_view, detail::default_customize_tag, detail::invalid_customize_tag>;

// Default customize.
  inline constexpr auto default_tag = detail::default_customize_tag{};
// Invalid customize.
  inline constexpr auto invalid_tag = detail::invalid_customize_tag{};

// If need custom names for enum, add specialization enum_name for necessary enum type.
  template <typename E>
  constexpr customize_t enum_name(E) noexcept {
    return default_tag;
  }

// If need custom type name for enum, add specialization enum_type_name for necessary enum type.
  template <typename E>
  constexpr customize_t enum_type_name() noexcept {
    return default_tag;
  }

}//namespace rendu

#endif //RENDU_ENUM_CUSTOMIZE_H_
