#ifndef RENDU_TRAITS_DETECTOR_H_
#define RENDU_TRAITS_DETECTOR_H_

#include <type_traits>
#include <concepts>

namespace rendu::traits
{
    namespace Details
    {
        struct Nullable {
            Nullable() = delete;
            ~Nullable() = delete;
            Nullable(Nullable const&) = delete;
            void operator=(Nullable const&) = delete;
        };

        template <typename Default, typename, template <typename...> typename, typename...>
        struct detector {
            using value_type = std::false_type;
            using storage_type = Default;
        };

        template <typename Default, template <typename...> typename Expression, typename... Args>
        struct detector<Default, std::void_t<Expression<Args...>>, Expression, Args...> {
            using value_type = std::true_type;
            using storage_type = Expression<Args...>;
        };
    }

    template <template <typename...> typename Expression, typename... Args>
    using IsDetected = typename Details::detector<Details::Nullable, void, Expression, Args...>::value_type;

    template <template <typename...> typename Expression, typename... Args>
    using DetectedType = typename Details::detector<Details::Nullable, void, Expression, Args...>::storage_type;

    template <typename Default, template <typename...> typename Expression, typename... Args>
    using IsDetectedOr = typename Details::detector<Default, void, Expression, Args...>::value_type;

    template <typename Default, template <typename...> typename Expression, typename... Args>
    using DetectedOrType = typename Details::detector<Default, void, Expression, Args...>::storage_type;
}


#endif // RENDU_TRAITS_DETECTOR_H_