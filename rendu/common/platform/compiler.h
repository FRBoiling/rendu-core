#ifndef RENDU_COMMON_PLATFORM_COMPILER_H_
#define RENDU_COMMON_PLATFORM_COMPILER_H_

/* ----------- [Compiler detect] ----------- */
#if defined(__clang__)
    #define RENDU_COMPILER_NAME    "Clang"
    #define RENDU_STANDARD_VER     __cplusplus
    #define RENDU_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define RENDU_COMPILER_NAME    "GCC"
    #define RENDU_STANDARD_VER     __cplusplus
    #define RENDU_COMPILER_GCC
#elif defined(_MSC_VER)
    #define RENDU_COMPILER_NAME    "MSVC"
    #define RENDU_STANDARD_VER     _MSVC_LANG
    #define RENDU_COMPILER_MSVC
#else
    #error Unsupported compiler
#endif

//#if RENDU_STANDARD_VER <= 201703L
//    #error Framework support only C++20 and higher!
//#endif

#endif  // RENDU_COMMON_PLATFORM_COMPILER_H_
