#ifndef RENDU_COMMON_PLATFORM_PLATFORM_H_
#define RENDU_COMMON_PLATFORM_PLATFORM_H_

/* ----------- [platform detect] ----------- */
#if defined(_WIN32) || defined(_WIN64)
    #define RENDU_PLATFORM_NAME    "Windows"

    #define RENDU_PLATFORM_WIN
    #if defined(_WIN32)
        #define RENDU_PLATFORM_WIN32
    #elif defined(_WIN64)
        #define RENDU_PLATFORM_WIN64
    #endif

    #include <platform/windows/windows.h>

#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__)
    #define RENDU_PLATFORM_NAME    "Linux"
    #define RENDU_PLATFORM_LINUX

    #include "unix/unix.h"
#else
    #error Unsupported platform
#endif

// Definition
#define RENDU_PLUGIN_API           extern "C" RENDU_API_EXPORT

#endif  // RENDU_COMMON_PLATFORM_PLATFORM_H_
