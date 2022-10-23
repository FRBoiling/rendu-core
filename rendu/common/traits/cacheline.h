#ifndef RENDU_TRAITS_CACHELINE_H_
#define RENDU_TRAITS_CACHELINE_H_

#include <new>

namespace rendu::traits {
#ifdef __cpp_lib_hardware_interference_size
    static constexpr inline std::size_t Cacheline = std::hardware_destructive_interference_size;
#else
    static constexpr inline std::size_t Cacheline = 64;
#endif
}

#endif // RENDU_TRAITS_CACHELINE_H_