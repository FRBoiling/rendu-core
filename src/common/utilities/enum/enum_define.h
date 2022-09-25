/*
* Created by boil on 2022/9/24.
*/

#ifndef RENDU_ENUM_DEFINE_H_
#define RENDU_ENUM_DEFINE_H_

#include <optional>
#include <string>
#include <string_view>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunknown-warning-option"
#  pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // May be used uninitialized 'return {};'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'StaticString<N>::chars_' is uninitialized.
#  pragma warning(disable : 28020) // Arithmetic overflow: Using operator '-' on a 4 byte value and then casting the result to a 8 byte value.
#  pragma warning(disable : 26451) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call.
#  pragma warning(disable : 4514) // Unreferenced inline function has been removed.
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
  using std::optional;
  using std::string_view;
  using std::string;

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

  template<typename... Es>
  [[nodiscard]] constexpr auto enum_fuse(Es... values) noexcept {
    static_assert((std::is_enum_v<std::decay_t<Es>> && ...), "rendu::enum_fuse requires enum type.");
    static_assert(sizeof...(Es) >= 2, "rendu::enum_fuse requires at least 2 values.");
    static_assert((log2(enum_count<std::decay_t<Es>>() + 1) + ...) <= (sizeof(std::uintmax_t) * 8),
                  "rendu::enum_fuse does not work for large enums");
#if defined(RENDU_ENUM_NO_TYPESAFE_ENUM_FUSE)
    const auto fuse = fuse_enum<std::decay_t<Es>...>(values...);
#else
    const auto fuse = typesafe_fuse_enum<std::decay_t<Es>...>(values...);
#endif
    return assert(fuse), fuse;
  }

} // namespace rendu

#endif //RENDU_ENUM_DEFINE_H_
