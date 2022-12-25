#ifndef RENDU_TRAITS_POWER_OF2_H_
#define RENDU_TRAITS_POWER_OF2_H_

#include <cstddef>

namespace rendu::traits
{
    template <std::size_t Size>
    static constexpr auto PowerOf2 = []{
        constexpr auto isPowerOf2 = Size && !(Size & (Size - 1));
        if constexpr(isPowerOf2) {
            return Size;
        } else {
            auto value = Size;
            value |= value >> 1;
            value |= value >> 2;
            value |= value >> 4;
            value |= value >> 8;
            value |= value >> 16;
            return ++value;
        }
    }();
}

#endif // RENDU_TRAITS_POWER_OF2_H_