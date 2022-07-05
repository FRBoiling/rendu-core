// This file is part of the rendu-core Project. See AUTHORS file for Copyright information
// Created by Boil on 2022/7/6.
//

#ifndef RENDU_ERRORS_H
#define RENDU_ERRORS_H
#include "Define.h"
#include <string>

namespace Trinity
{
[[noreturn]] TC_COMMON_API void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message);
[[noreturn]] TC_COMMON_API void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message, char const* format, ...) ATTR_PRINTF(6, 7);

[[noreturn]] TC_COMMON_API void Fatal(char const* file, int line, char const* function, char const* message, ...) ATTR_PRINTF(4, 5);

[[noreturn]] TC_COMMON_API void Error(char const* file, int line, char const* function, char const* message);

[[noreturn]] TC_COMMON_API void Abort(char const* file, int line, char const* function);
[[noreturn]] TC_COMMON_API void Abort(char const* file, int line, char const* function, char const* message, ...);

TC_COMMON_API void Warning(char const* file, int line, char const* function, char const* message);

[[noreturn]] TC_COMMON_API void AbortHandler(int sigval);

} // namespace Trinity

TC_COMMON_API std::string GetDebugInfo();

#if TRINITY_COMPILER == TRINITY_COMPILER_MICROSOFT
#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define ASSERT_END __pragma(warning(pop))
#else
#define ASSERT_BEGIN
#define ASSERT_END
#endif

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
#define EXCEPTION_ASSERTION_FAILURE 0xC0000420L
#endif

#define WPAssert(cond, ...) ASSERT_BEGIN do { if (!(cond)) Trinity::Assert(__FILE__, __LINE__, __FUNCTION__, GetDebugInfo(), #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPAssert_NODEBUGINFO(cond, ...) ASSERT_BEGIN do { if (!(cond)) Trinity::Assert(__FILE__, __LINE__, __FUNCTION__, "", #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPFatal(cond, ...) ASSERT_BEGIN do { if (!(cond)) Trinity::Fatal(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPError(cond, msg) ASSERT_BEGIN do { if (!(cond)) Trinity::Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPWarning(cond, msg) ASSERT_BEGIN do { if (!(cond)) Trinity::Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPAbort() ASSERT_BEGIN do { Trinity::Abort(__FILE__, __LINE__, __FUNCTION__); } while(0) ASSERT_END
#define WPAbort_MSG(msg, ...) ASSERT_BEGIN do { Trinity::Abort(__FILE__, __LINE__, __FUNCTION__, (msg), ##__VA_ARGS__); } while(0) ASSERT_END

#ifdef PERFORMANCE_PROFILING
#define ASSERT(cond, ...) ((void)0)
#define ASSERT_NODEBUGINFO(cond, ...) ((void)0)
#else
#define ASSERT WPAssert
#define ASSERT_NODEBUGINFO WPAssert_NODEBUGINFO
#endif

#define ASSERT_WITH_SIDE_EFFECTS WPAssert

#define ABORT WPAbort
#define ABORT_MSG WPAbort_MSG

template <typename T>
inline T* ASSERT_NOTNULL_IMPL(T* pointer, char const* expr)
{
  ASSERT(pointer, "%s", expr);
  return pointer;
}

#define ASSERT_NOTNULL(pointer) ASSERT_NOTNULL_IMPL(pointer, #pointer)

#endif//RENDU_ERRORS_H
