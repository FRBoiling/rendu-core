/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_COMMON_DEFINE_H_
#define RENDU_COMMON_DEFINE_H_

#ifndef RD_EXPORT
#    if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#        define RD_EXPORT __declspec(dllexport)
#        define RD_IMPORT __declspec(dllimport)
#        define RD_HIDDEN
#    elif defined __GNUC__ && __GNUC__ >= 4
#        define RD_EXPORT __attribute__((visibility("default")))
#        define RD_IMPORT __attribute__((visibility("default")))
#        define RD_HIDDEN __attribute__((visibility("hidden")))
#    else /* Unsupported compiler */
#        define RD_EXPORT
#        define RD_IMPORT
#        define RD_HIDDEN
#    endif
#endif

#ifndef RD_API
#    if defined RD_API_EXPORT
#        define RD_API RD_EXPORT
#    elif defined RD_API_IMPORT
#        define RD_API RD_IMPORT
#    else /* No API */
#        define RD_API
#    endif
#endif

#define RD_NAMESPACE_BEGIN  namespace rendu {
#define RD_NAMESPACE_END } // namespace rendu

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <iterator>
#include <limits>

RD_NAMESPACE_BEGIN

    using int8 = std::int8_t;
    using uint8 = std::uint8_t;
    using int16 = std::int16_t;
    using uint16 = std::uint16_t;
    using int32 = std::int32_t;
    using uint32 = std::uint32_t;
    using int64 = std::int64_t;
    using uint64 = std::uint64_t;

    using byte = std::byte;
    using size = std::size_t;

    using string = std::string;

RD_NAMESPACE_END

#ifdef _WIN32
#include <crtdbg.h> // Windows-specific header for _ASSERT
#else
#include <cassert> // Standard header for assert
#endif

#ifdef _WIN32
#define ASSERT(expr) _ASSERT(expr)
#else
#define ASSERT(expr) assert(expr)
#endif


#endif //RENDU_COMMON_DEFINE_H_
