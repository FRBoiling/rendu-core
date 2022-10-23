#ifndef RENDU_TRAITS_CONDITIONAL_H_
#define RENDU_TRAITS_CONDITIONAL_H_

#include <type_traits>

namespace rendu::traits
{
    template <bool Condition, typename Success, typename Failure>
    using Conditional = std::conditional_t<Condition, Success, Failure>;
}

#endif // RENDU_TRAITS_CONDITIONAL_H_
