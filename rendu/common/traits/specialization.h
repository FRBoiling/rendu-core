#ifndef RENDU_TRAITS_SPECIALIZATION_H_
#define RENDU_TRAITS_SPECIALIZATION_H_

#include <type_traits>
#include <concepts>

namespace rendu::traits
{
    template <typename, template <typename...> typename>
    struct SpecializationOf : std::false_type {};

    template <template <typename...> typename Primary, typename... Args>
    struct SpecializationOf<Primary<Args...>, Primary> : std::true_type {};

    template <typename T, template <typename...> typename Primary>
    concept Specialization = SpecializationOf<T, Primary>::value;
}


#endif // RENDU_TRAITS_SPECIALIZATION_H_