//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDU_COMMON_DEFINE_H
#define RENDU_COMMON_DEFINE_H

#define RENDU_PLATFORM_WINDOWS 0
#define RENDU_PLATFORM_UNIX    1
#define RENDU_PLATFORM_APPLE   2

#if defined( _WIN32 )
#  define RENDU_PLATFORM RENDU_PLATFORM_WINDOWS
#elif defined( __APPLE__ )
#  define RENDU_PLATFORM RENDU_PLATFORM_APPLE
#else
#  define RENDU_PLATFORM RENDU_PLATFORM_UNIX
#endif

#define RENDU_COMPILER_MICROSOFT 0
#define RENDU_COMPILER_GNU       1
#define RENDU_COMPILER_INTEL     2

#ifdef _MSC_VER
#  define RENDU_COMPILER RENDU_COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
#  define RENDU_COMPILER RENDU_COMPILER_INTEL
#elif defined( __GNUC__ )
#  define RENDU_COMPILER RENDU_COMPILER_GNU
#  define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PARCHLEVEL__)
#else
#  error "FATAL ERROR: Unknown compiler."
#endif

#if RENDU_COMPILER == RENDU_COMPILER_GNU
#  if !defined(__STDC_FORMAT_MACROS)
#    define __STDC_FORMAT_MACROS
#  endif
#  if !defined(__STDC_CONSTANT_MACROS)
#    define __STDC_CONSTANT_MACROS
#  endif
#  if !defined(_GLIBCXX_USE_NANOSLEEP)
#    define _GLIBCXX_USE_NANOSLEEP
#  endif
#  if defined(HELGRIND)
#    include <valgrind/helgrind.h>
#    undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE
#    undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER
#    define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A) ANNOTATE_HAPPENS_BEFORE(A)
#    define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A)  ANNOTATE_HAPPENS_AFTER(A)
#  endif
#  if defined(VALGRIND)
#    include <valgrind/memcheck.h>
#  endif
#endif

#include <cstddef>
#include <cinttypes>
#include <climits>

#define RENDU_LITTLEENDIAN 0
#define RENDU_BIGENDIAN    1

#if !defined(RENDU_ENDIAN)
#  if defined (BOOST_BIG_ENDIAN)
#    define RENDU_ENDIAN RENDU_BIGENDIAN
#  else
#    define RENDU_ENDIAN RENDU_LITTLEENDIAN
#  endif
#endif

#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
#  define RENDU_PATH_MAX 260
#else // RENDU_PLATFORM != RENDU_PLATFORM_WINDOWS
#  define RENDU_PATH_MAX PATH_MAX
#endif // RENDU_PLATFORM

#if !defined(COREDEBUG)
#  define RENDU_INLINE inline
#else //COREDEBUG
#  if !defined(RENDU_DEBUG)
#    define RENDU_DEBUG
#  endif //RENDU_DEBUG
#  define RENDU_INLINE
#endif //!COREDEBUG

#if RENDU_COMPILER == RENDU_COMPILER_GNU
#  define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
#else //RENDU_COMPILER != RENDU_COMPILER_GNU
#  define ATTR_PRINTF(F, V)
#endif //RENDU_COMPILER == RENDU_COMPILER_GNU

#ifdef RENDU_API_USE_DYNAMIC_LINKING
#  if RENDU_COMPILER == RENDU_COMPILER_MICROSOFT
#    define RC_API_EXPORT __declspec(dllexport)
#    define RC_API_IMPORT __declspec(dllimport)
#  elif RENDU_COMPILER == RENDU_COMPILER_GNU
#    define RC_API_EXPORT __attribute__((visibility("default")))
#    define RC_API_IMPORT
#  else
#    error compiler not supported!
#  endif
#else
#  define RC_API_EXPORT
#  define RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_COMMON
#  define RC_COMMON_API RC_API_EXPORT
#else
#  define RC_COMMON_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_PROTO
#  define RC_PROTO_API RC_API_EXPORT
#else
#  define RC_PROTO_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_DATABASE
#  define RC_DATABASE_API RC_API_EXPORT
#else
#  define RC_DATABASE_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_NETWORK
#  define RC_NETWORK_API RC_API_EXPORT
#else
#  define RC_NETWORK_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_SHARED
#  define RC_SHARED_API RC_API_EXPORT
#else
#  define RC_SHARED_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_GAME
#  define RC_GAME_API RC_API_EXPORT
#else
#  define RC_GAME_API RC_API_IMPORT
#endif

#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARG(str) static_cast<int>((str).length()), (str).data()

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

// 添加命名空间宏定义
#define BEGIN_NAMESPACE_COMMON namespace Rendu {
#define END_NAMESPACE_COMMON } // namespace Rendu

#define BEGIN_NAMESPACE_ECS namespace Rendu {
#define END_NAMESPACE_ECS } // namespace Rendu

#endif // RENDU_COMMON_DEFINE_H
