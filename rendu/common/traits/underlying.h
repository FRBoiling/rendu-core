#ifndef RENDU_TRAITS_UNDERLYING_H_
#define RENDU_TRAITS_UNDERLYING_H_

#include <type_traits>

namespace rendu::traits
{
    template <typename T>
    using Underlying = std::underlying_type_t<T>;
}


#endif // RENDU_TRAITS_IDENTITY_H_