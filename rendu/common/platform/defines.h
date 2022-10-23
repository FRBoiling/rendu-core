#ifndef RENDU_COMMON_PLATFORM_DEFINES_H_
#define RENDU_COMMON_PLATFORM_DEFINES_H_

#include "compiler.h"
#include "processor.h"

/* ----------- [Debug detect] ----------- */
#if !defined(NDEBUG)
    #define RENDU_DEBUG
#endif

/* ----------- [Diagnostic Pragma] ----------- */
#if defined(RENDU_COMPILER_CLANG)
    #define RENDU_DIAGNOSTIC_CLANG_PUSH        _Pragma("clang diagnostic push")
    #define RENDU_DIAGNOSTIC_CLANG_POP         _Pragma("clang diagnostic pop")
    #define RENDU_DIAGNOSTIC_CLANG_IGNORE(id)  _Pragma("clang diagnostic ignored " #id)
#else
    #define RENDU_DIAGNOSTIC_CLANG_PUSH
    #define RENDU_DIAGNOSTIC_CLANG_POP
    #define RENDU_DIAGNOSTIC_CLANG_IGNORE(id)
#endif

#if defined(RENDU_COMPILER_GCC)
    #define RENDU_DIAGNOSTIC_GCC_PUSH          _Pragma("GCC diagnostic push")
    #define RENDU_DIAGNOSTIC_GCC_POP           _Pragma("GCC diagnostic pop")
    #define RENDU_DIAGNOSTIC_GCC_IGNORE(id)    _Pragma("GCC diagnostic ignored " #id)
#else
    #define RENDU_DIAGNOSTIC_GCC_PUSH
    #define RENDU_DIAGNOSTIC_GCC_POP
    #define RENDU_DIAGNOSTIC_GCC_IGNORE(id)
#endif

#if defined(RENDU_COMPILER_MSVC)
    #define RENDU_DIAGNOSTIC_MSVC_PUSH         _Pragma("warning(push)")
    #define RENDU_DIAGNOSTIC_MSVC_POP          _Pragma("warning(pop)")
    #define RENDU_DIAGNOSTIC_MSVC_IGNORE(id)   _Pragma("warning(disable: id)")
#else
    #define RENDU_DIAGNOSTIC_MSVC_PUSH
    #define RENDU_DIAGNOSTIC_MSVC_POP
    #define RENDU_DIAGNOSTIC_MSVC_IGNORE(id)
#endif

#if defined(RENDU_COMPILER_CLANG)
    #define RENDU_DIAGNOSTIC_PUSH              RENDU_DIAGNOSTIC_CLANG_PUSH
    #define RENDU_DIAGNOSTIC_POP               RENDU_DIAGNOSTIC_CLANG_POP
    #define RENDU_DIAGNOSTIC_IGNORE(id)        RENDU_DIAGNOSTIC_CLANG_IGNORE(id)
#elif defined(RENDU_COMPILER_GCC)
    #define RENDU_DIAGNOSTIC_PUSH              RENDU_DIAGNOSTIC_GCC_PUSH
    #define RENDU_DIAGNOSTIC_POP               RENDU_DIAGNOSTIC_GCC_POP
    #define RENDU_DIAGNOSTIC_IGNORE(id)        RENDU_DIAGNOSTIC_GCC_IGNORE(id)
#elif defined(RENDU_COMPILER_MSVC)
    #define RENDU_DIAGNOSTIC_PUSH              RENDU_DIAGNOSTIC_MSVC_PUSH
    #define RENDU_DIAGNOSTIC_POP               RENDU_DIAGNOSTIC_MSVC_POP
    #define RENDU_DIAGNOSTIC_IGNORE(id)        RENDU_DIAGNOSTIC_MSVC_IGNORE(id)
#else
    #define RENDU_DIAGNOSTIC_PUSH
    #define RENDU_DIAGNOSTIC_POP
    #define RENDU_DIAGNOSTIC_IGNORE(id)
#endif

/* ----------- [Features] ----------- */
#if defined(RENDU_DEBUG)
    #if defined(RENDU_COMPILER_GCC) || defined(RENDU_COMPILER_CLANG)
        #define RENDU_FORCEINLINE              __attribute__((always_inline)
    #elif defined(RENDU_COMPILER_MSVC)
        #define RENDU_FORCEINLINE              __forceinline
    #endif
#else
    #define RENDU_FORCEINLINE
#endif

#if defined(RENDU_COMPILER_CLANG) || defined(RENDU_COMPILER_GCC)
    #define RENDU_FUNCTION             __PRETTY_FUNCTION__
#else
    #define RENDU_FUNCTION             __FUNCSIG__
#endif

#if defined(RENDU_COMPILER_CLANG) || defined(RENDU_COMPILER_GCC) || defined(RENDU_COMPILER_MSVC)
    #define RENDU_RESTRICT             __restrict
#else
    #define RENDU_RESTRICT
#endif

#if defined(RENDU_COMPILER_CLANG)
    #define RENDU_OPTIMIZATION_ENABLE          _Pragma("clang optimize on")
    #define RENDU_OPTIMIZATION_DISABLE         _Pragma("clang optimize off")
#elif defined(RENDU_COMPILER_GCC)
    #define RENDU_OPTIMIZATION_ENABLE          _Pragma("GCC pop_options")
    #define RENDU_OPTIMIZATION_DISABLE         _Pragma("GCC push_options") \
                                                _Pragma("GCC optimize (\"-O0\")")
#elif defined(RENDU_COMPILER_MSVC)
    #define RENDU_OPTIMIZATION_ENABLE          _Pragma("optimize(\"\", on)")
    #define RENDU_OPTIMIZATION_DISABLE         _Pragma("optimize(\"\", off)")
#else
    #define RENDU_OPTIMIZATION_ENABLE
    #define RENDU_OPTIMIZATION_DISABLE
#endif

#if __has_cpp_attribute(likely)
    #define RENDU_LIKELY               [[likely]]
#else
    #define RENDU_LIKELY
#endif

#if __has_cpp_attribute(unlikely)
    #define RENDU_UNLIKELY             [[unlikely]]
#else
    #define RENDU_UNLIKELY
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1929)
    #define RENDU_NO_UNIQUE_ADDRESS    [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
    #define RENDU_NO_UNIQUE_ADDRESS    [[no_unique_address]]
#else
    #define RENDU_NO_UNIQUE_ADDRESS
#endif

//#if defined(RENDU_STANDARD_CPP11_OR_GREATER)
//    #define RENDU_NOEXCEPT           noexcept
//    #define RENDU_CONSTEXPR          constexpr
//    #define RENDU_FINAL              final
//#else
//    #define RENDU_NOEXCEPT           throw()
//    #define RENDU_CONSTEXPR
//    #define RENDU_FINAL
//#endif

//#if defined(RENDU_STANDARD_CPP17_OR_GREATER)
//    #define RENDU_NODISCARD          [[nodiscard]]
//#else
//    #define RENDU_NODISCARD
//#endif

//#if defined(RENDU_STANDARD_CPP20)
//    #define RENDU_NODISCARD_MSG(msg) [[nodiscard(msg)]]
//#endif

//#if defined(RENDU_COMPILER_MSVC)
//    #define RENDU_CONSTEVAL          consteval
//#elif defined(RENDU_COMPILER_GCC) || defined(RENDU_COMPILER_CLANG)
//    #define RENDU_CONSTEVAL          constexpr
//#endif

#if defined(RENDU_COMPILER_MSVC)
    #include <immintrin.h>
    #pragma intrinsic(_mm_pause)
    #define RENDU_PROCESSOR_YIELD()    _mm_pause()
#elif defined(RENDU_COMPILER_GCC) || defined(RENDU_COMPILER_CLANG)
    #define RENDU_PROCESSOR_YIELD()    __builtin_ia32_pause()
#endif

//#if defined(RENDU_PROCESSOR_AMD64) || defined(RENDU_PROCESSOR_X86)
//    #if defined(RENDU_COMPILER_MSVC)
//        #include <immintrin.h>
//        #pragma intrinsic(_mm_pause)
//        #define RENDU_PROCESSOR_YIELD()        _mm_pause()
//    #elif defined(RENDU_COMPILER_GCC) || defined(RENDU_COMPILER_CLANG)
//        #if __has_builtin (__builtin_ia32_pause)
//            #define RENDU_PROCESSOR_YIELD()    __builtin_ia32_pause()
//        #else
//            #define RENDU_PROCESSOR_YIELD()    __asm__ __volatile__ ("pause")
//        #endif
//    #endif
//#elif defined(RENDU_PROCESSOR_ARM)
//    #define RENDU_PROCESSOR_YIELD()            __asm__ __volatile__ ("isb" ::: "memory")
//#elif defined(RENDU_PROCESSOR_PPC64)
//    #define RENDU_PROCESSOR_YIELD()            __asm__ __volatile__ ("or 27,27,27" ::: "memory")
//#elif defined(RENDU_PROCESSOR_IA64)
//    #define RENDU_PROCESSOR_YIELD()            __asm__ __volatile__ ("hint @pause")
//#elif defined(RENDU_PROCESSOR_SPARC)
//    #define RENDU_PROCESSOR_YIELD()            __asm__ __volatile__ ("nop;");
//#endif

#define RENDU_NEW(type)                        new type
#define RENDU_NEW_NOTHROW(type)                new (std::nothrow) type
#define RENDU_NEW_ARRAY(type, size)            new type[size]
#define RENDU_NEW_ARRAY_NOTHROW(type, size)    new (std::nothrow) type[size]
#define RENDU_DELETE(type)                     delete type
#define RENDU_DELETE_ARRAY(type)               delete[] type

#define RENDU_CLASSNAME(type)                  #type
#define RENDU_CLASSNAME_RUNTIME(type)          typeid(type).name()

#endif  // RENDU_COMMON_PLATFORM_DEFINES_H_
