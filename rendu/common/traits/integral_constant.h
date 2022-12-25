#ifndef RENDU_TRAITS_INTEGRAL_CONSTANT_H_
#define RENDU_TRAITS_INTEGRAL_CONSTANT_H_

#include <type_traits>
#include <concepts>

namespace rendu::traits
{
    template <typename T>
    struct IsIntegralConstant : std::false_type {};

    template <typename T, T Value>
    struct IsIntegralConstant<std::integral_constant<T, Value>> : std::true_type {};

    template <typename T>
    concept IntegralConstant = IsIntegralConstant<T>::value;
}


#endif // RENDU_TRAITS_INTEGRAL_CONSTANT_H_