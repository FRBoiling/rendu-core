/*
* Created by boil on 2022/9/17.
*/

#ifndef RENDU_ENUM_FUSE_H_
#define RENDU_ENUM_FUSE_H_

#include "enum_detail.h"

namespace rendu {

// Returns a bijective mix of several enum values. This can be used to emulate 2D switch/case statements.
  template<typename... Es>
  [[nodiscard]] constexpr auto enum_fuse(Es... values) noexcept {
    static_assert((std::is_enum_v<std::decay_t<Es>> && ...), "rendu::enum_fuse requires enum type.");
    static_assert(sizeof...(Es) >= 2, "rendu::enum_fuse requires at least 2 values.");
    static_assert((detail::log2(enum_count<std::decay_t<Es>>() + 1) + ...) <= (sizeof(std::uintmax_t) * 8),
                  "rendu::enum_fuse does not work for large enums");
#if defined(RENDU_ENUM_NO_TYPESAFE_ENUM_FUSE)
    const auto fuse = detail::fuse_enum<std::decay_t<Es>...>(values...);
#else
    const auto fuse = detail::typesafe_fuse_enum<std::decay_t<Es>...>(values...);
#endif
    return assert(fuse), fuse;
  }

} // namespace rendu

#endif // RENDU_ENUM_FUSE_H_
