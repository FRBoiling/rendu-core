#ifndef RENDU_TRAITS_REMOVE_H_
#define RENDU_TRAITS_REMOVE_H_

#include <rendu/traits/Conditional.hpp>

namespace rendu::traits
{
    template <typename T>
    using RemoveConst = std::remove_const_t<T>;

    template <typename T>
    using RemoveVol = std::remove_volatile_t<T>;

    template <typename T>
    using RemoveRef = std::remove_reference_t<T>;

    template <typename T>
    using RemovePtr = std::remove_pointer_t<T>;

    template <typename T>
    using RemoveCV = std::remove_cv_t<T>;

    template <typename T>
    using RemoveCVR = std::remove_cvref_t<T>;

    template <typename T>
    using RemoveCVRP = Conditional<std::is_array_v<T> || std::is_function_v<T>, std::decay_t<T>,
        Conditional<std::is_pointer_v<T>, RemoveCVR<RemovePtr<T>>, RemoveCVR<T>>>;
}

#endif // RENDU_TRAITS_REMOVE_H_