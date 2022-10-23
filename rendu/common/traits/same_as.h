#ifndef RENDU_TRAITS_SAMEAS_H_
#define RENDU_TRAITS_SAMEAS_H_

#include <type_traits>

namespace rendu::traits {
    template <typename T, typename P>
    static constexpr auto SameAs = std::is_same_v<T, P>;
}

#endif // RENDU_TRAITS_SAMEAS_H_