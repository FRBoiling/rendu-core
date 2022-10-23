#ifndef RENDU_TRAITS_SCOPEDENUM_H_
#define RENDU_TRAITS_SCOPEDENUM_H_

#include <type_traits>
#include <concepts>

namespace rendu::traits
{
    template <typename T>
    struct IsScopedEnum : std::conjunction<std::is_enum<T>, std::negation<std::is_convertible<T, std::int32_t>>>::type {};

    template <typename T>
    concept ScopedEnum = IsScopedEnum<T>::value;
}


#endif // RENDU_TRAITS_SCOPEDENUM_H_