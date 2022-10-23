#ifndef RENDU_TRAITS_SAMEALL_H_
#define RENDU_TRAITS_SAMEALL_H_

#include <type_traits>

namespace rendu::traits {
    template <typename First, typename... Other>
    static constexpr auto SameAll = std::conjunction_v<std::is_same<First, Other>...>;
}

#endif // RENDU_TRAITS_SAMEALL_H_