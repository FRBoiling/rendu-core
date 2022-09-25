/*
* Created by boil on 2022/9/24.
*/

#ifndef RENDU_ENUM_DEFINE_H_
#define RENDU_ENUM_DEFINE_H_

#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <type_traits>
#include <utility>
#include <variant>

#if defined(RENDU_ENUM_CONFIG_FILE)
#include RENDU_ENUM_CONFIG_FILE
#endif

#if !defined(RENDU_ENUM_USING_ALIAS_OPTIONAL)
#include <optional>
#endif
#if !defined(RENDU_ENUM_USING_ALIAS_STRING)
#include <string>
#endif
#if !defined(RENDU_ENUM_USING_ALIAS_STRING_VIEW)
#include <string_view>
#endif

// Enum value must be greater or equals than RENDU_ENUM_RANGE_MIN. By default RENDU_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro RENDU_ENUM_RANGE_MIN.
#if !defined(RENDU_ENUM_RANGE_MIN)
#  define RENDU_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than RENDU_ENUM_RANGE_MAX. By default RENDU_ENUM_RANGE_MAX = 128.
// If need another max range for all enum types by default, redefine the macro RENDU_ENUM_RANGE_MAX.
#if !defined(RENDU_ENUM_RANGE_MAX)
#  define RENDU_ENUM_RANGE_MAX 128
#endif

namespace rendu {
// If need another optional type, define the macro RENDU_ENUM_USING_ALIAS_OPTIONAL.
#if defined(RENDU_ENUM_USING_ALIAS_OPTIONAL)
  RENDU_ENUM_USING_ALIAS_OPTIONAL
#else
  using std::optional;
#endif

// If need another string_view type, define the macro RENDU_ENUM_USING_ALIAS_STRING_VIEW.
#if defined(RENDU_ENUM_USING_ALIAS_STRING_VIEW)
  RENDU_ENUM_USING_ALIAS_STRING_VIEW
#else
  using std::string_view;
#endif

// If need another string type, define the macro RENDU_ENUM_USING_ALIAS_STRING.
#if defined(RENDU_ENUM_USING_ALIAS_STRING)
  RENDU_ENUM_USING_ALIAS_STRING
#else
  using std::string;
#endif


#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunknown-warning-option"
#  pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#elif defined(__GNUC__)
  #  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // May be used uninitialized 'return {};'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'static_string<N>::chars_' is uninitialized.
#  pragma warning(disable : 28020) // Arithmetic overflow: Using operator '-' on a 4 byte value and then casting the result to a 8 byte value.
#  pragma warning(disable : 26451) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call.
#  pragma warning(disable : 4514) // Unreferenced inline function has been removed.
#endif

// Checks rendu compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  RENDU_ENUM_SUPPORTED
#  define RENDU_ENUM_SUPPORTED 1
#endif

// Checks rendu compiler aliases compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  undef  RENDU_ENUM_SUPPORTED_ALIASES
#  define RENDU_ENUM_SUPPORTED_ALIASES 1
#endif

// Enum value must be greater or equals than RENDU_ENUM_RANGE_MIN. By default RENDU_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro RENDU_ENUM_RANGE_MIN.
#if !defined(RENDU_ENUM_RANGE_MIN)
#  define RENDU_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than RENDU_ENUM_RANGE_MAX. By default RENDU_ENUM_RANGE_MAX = 128.
// If need another max range for all enum types by default, redefine the macro RENDU_ENUM_RANGE_MAX.
#if !defined(RENDU_ENUM_RANGE_MAX)
#  define RENDU_ENUM_RANGE_MAX 128
#endif

} // namespace rendu

#endif //RENDU_ENUM_DEFINE_H_
