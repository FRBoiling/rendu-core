#ifndef RENDU_TRAITS_ARGUMENTS_H_
#define RENDU_TRAITS_ARGUMENTS_H_

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace rendu::traits {
    template <typename... Args>
    struct Arguments {
        static constexpr auto Size = sizeof...(Args);
        static constexpr auto Orphan = !Size;
        static constexpr auto Single = (Size == 1);
        static constexpr auto Double = (Size == 2);

        using Pack = std::tuple<Args...>;

        template <std::size_t Index>
        requires (Index < Size)
        using Get = std::tuple_element_t<Index, Pack>;
    };
}

#endif // RENDU_TRAITS_ARGUMENTS_H_
