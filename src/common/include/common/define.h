//
// Created by FRee2 on 2025/10/18.
//

// ============================================================================
// define.h - 平台、编译器和通用类型定义
// ============================================================================

#ifndef RENDU_COMMON_DEFINE_H
#define RENDU_COMMON_DEFINE_H

// ============================================================================
// 平台类型定义
// ============================================================================

#define RENDU_PLATFORM_WINDOWS 0
#define RENDU_PLATFORM_UNIX    1
#define RENDU_PLATFORM_APPLE   2

#if defined(_WIN32)
  #define RENDU_PLATFORM RENDU_PLATFORM_WINDOWS
#elif defined(__APPLE__)
  #define RENDU_PLATFORM RENDU_PLATFORM_APPLE
#else
  #define RENDU_PLATFORM RENDU_PLATFORM_UNIX
#endif

// ============================================================================
// 编译器类型定义
// ============================================================================

#define RENDU_COMPILER_MICROSOFT 0
#define RENDU_COMPILER_GNU       1
#define RENDU_COMPILER_INTEL     2

#ifdef _MSC_VER
  #define RENDU_COMPILER RENDU_COMPILER_MICROSOFT
#elif defined(__INTEL_COMPILER)
  #define RENDU_COMPILER RENDU_COMPILER_INTEL
#elif defined(__GNUC__)
  #define RENDU_COMPILER RENDU_COMPILER_GNU
  #define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
  #error "FATAL ERROR: Unknown compiler."
#endif

// ============================================================================
// GCC 编译器特定设置
// ============================================================================

#if RENDU_COMPILER == RENDU_COMPILER_GNU
  // 启用标准格式化宏
  #if !defined(__STDC_FORMAT_MACROS)
    #define __STDC_FORMAT_MACROS
  #endif
  #if !defined(__STDC_CONSTANT_MACROS)
    #define __STDC_CONSTANT_MACROS
  #endif
  #if !defined(_GLIBCXX_USE_NANOSLEEP)
    #define _GLIBCXX_USE_NANOSLEEP
  #endif
  // Helgrind 线程分析支持
  #if defined(HELGRIND)
    #include <valgrind/helgrind.h>
    #undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE
    #undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER
    #define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A) ANNOTATE_HAPPENS_BEFORE(A)
    #define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A)  ANNOTATE_HAPPENS_AFTER(A)
  #endif
  // Valgrind 内存检查支持
  #if defined(VALGRIND)
    #include <valgrind/memcheck.h>
  #endif
#endif

// ============================================================================
// 标准头文件包含
// ============================================================================

#include <cstddef>
#include <cinttypes>
#include <climits>

// ============================================================================
// 字节序定义
// ============================================================================

#define RENDU_LITTLEENDIAN 0
#define RENDU_BIGENDIAN    1

#if !defined(RENDU_ENDIAN)
  #if defined(BOOST_BIG_ENDIAN)
    #define RENDU_ENDIAN RENDU_BIGENDIAN
  #else
    #define RENDU_ENDIAN RENDU_LITTLEENDIAN
  #endif
#endif

// ============================================================================
// 路径最大长度定义
// ============================================================================

#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
  #define RENDU_PATH_MAX 260
#else
  #define RENDU_PATH_MAX PATH_MAX
#endif

// ============================================================================
// 内联函数控制
// ============================================================================

#if !defined(COREDEBUG)
  #define RENDU_INLINE inline
#else
  #if !defined(RENDU_DEBUG)
    #define RENDU_DEBUG
  #endif
  #define RENDU_INLINE
#endif

// ============================================================================
// 格式化属性（用于 printf 风格函数检查）
// ============================================================================

#if RENDU_COMPILER == RENDU_COMPILER_GNU
  #define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
#else
  #define ATTR_PRINTF(F, V)
#endif

// ============================================================================
// API 导出/导入控制
// ============================================================================

// Common 层 API 定义
#if defined(RENDU_COMMON_EXPORT)
  #if defined(_WIN32) || defined(_WIN64)
    #define RENDU_COMMON_API __declspec(dllexport)
  #else
    #define RENDU_COMMON_API __attribute__((visibility("default")))
  #endif
#elif defined(RENDU_COMMON_EXPORT)
  #if defined(_WIN32) || defined(_WIN64)
    #define RENDU_COMMON_API __declspec(dllimport)
  #else
    #define RENDU_COMMON_API
  #endif
#else
  #define RENDU_COMMON_API
#endif

// Core 层 API 定义
#if defined(RENDU_CORE_EXPORT)
  #if defined(_WIN32) || defined(_WIN64)
    #define RENDU_CORE_API __declspec(dllexport)
  #else
    #define RENDU_CORE_API __attribute__((visibility("default")))
  #endif
#elif defined(RENDU_CORE_IMPORT)
  #if defined(_WIN32) || defined(_WIN64)
    #define RENDU_CORE_API __declspec(dllimport)
  #else
    #define RENDU_CORE_API
  #endif
#else
  #define RENDU_CORE_API
#endif

// ============================================================================
// 旧版 API 导出/导入控制（保持兼容性）
// ============================================================================

#ifdef RENDU_API_USE_DYNAMIC_LINKING
  #if RENDU_COMPILER == RENDU_COMPILER_MICROSOFT
    #define RC_API_EXPORT __declspec(dllexport)
    #define RC_API_IMPORT __declspec(dllimport)
  #elif RENDU_COMPILER == RENDU_COMPILER_GNU
    #define RC_API_EXPORT __attribute__((visibility("default")))
    #define RC_API_IMPORT
  #else
    #error "Compiler not supported!"
  #endif
#else
  #define RC_API_EXPORT
  #define RC_API_IMPORT
#endif

// ============================================================================
// 各模块 API 定义
// ============================================================================

#ifdef RENDU_API_EXPORT_COMMON
  #define RC_COMMON_API RC_API_EXPORT
#else
  #define RC_COMMON_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_PROTO
  #define RC_PROTO_API RC_API_EXPORT
#else
  #define RC_PROTO_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_DATABASE
  #define RC_DATABASE_API RC_API_EXPORT
#else
  #define RC_DATABASE_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_NETWORK
  #define RC_NETWORK_API RC_API_EXPORT
#else
  #define RC_NETWORK_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_SHARED
  #define RC_SHARED_API RC_API_EXPORT
#else
  #define RC_SHARED_API RC_API_IMPORT
#endif

#ifdef RENDU_API_EXPORT_GAME
  #define RC_GAME_API RC_API_EXPORT
#else
  #define RC_GAME_API RC_API_IMPORT
#endif

// ============================================================================
// 整数格式化宏
// ============================================================================

#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

// ============================================================================
// 字符串视图格式化宏
// ============================================================================

#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARG(str) static_cast<int>((str).length()), (str).data()

// ============================================================================
// 通用整数类型定义
// ============================================================================

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

// ============================================================================
// 字节类型定义
// ============================================================================

typedef unsigned char byte;

// ============================================================================
// 命名空间宏定义
// ============================================================================

// Common 层命名空间引用
#define COMMON_NAMESPACE Rendu

#define BEGIN_NAMESPACE_COMMON namespace COMMON_NAMESPACE {
#define END_NAMESPACE_COMMON } // namespace COMMON_NAMESPACE

#endif // RENDU_COMMON_DEFINE_H
