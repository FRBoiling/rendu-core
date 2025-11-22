//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_ERRORS_H
#define RENDU_ERRORS_H

#include "common/define.h"
#include <string>

BEGIN_NAMESPACE_COMMON

    [[noreturn]] RC_COMMON_API void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message);
    [[noreturn]] RC_COMMON_API void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message, char const* format, ...) ATTR_PRINTF(6, 7);

    [[noreturn]] RC_COMMON_API void Fatal(char const* file, int line, char const* function, char const* message, ...) ATTR_PRINTF(4, 5);

    [[noreturn]] RC_COMMON_API void Error(char const* file, int line, char const* function, char const* message);

    [[noreturn]] RC_COMMON_API void Abort(char const* file, int line, char const* function);
    [[noreturn]] RC_COMMON_API void Abort(char const* file, int line, char const* function, char const* message, ...);

    RC_COMMON_API void Warning(char const* file, int line, char const* function, char const* message);

    [[noreturn]] RC_COMMON_API void AbortHandler(int sigval);

END_NAMESPACE_COMMON

RC_COMMON_API std::string GetDebugInfo();

#if RENDU_COMPILER == RENDU_COMPILER_MICROSOFT
#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define ASSERT_END __pragma(warning(pop))
#else
#define ASSERT_BEGIN
#define ASSERT_END
#endif

#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
#define EXCEPTION_ASSERTION_FAILURE 0xC0000420L
#endif

#define WPAssert(cond, ...) ASSERT_BEGIN do { if (!(cond)) [[unlikely]] Rendu::Assert(__FILE__, __LINE__, __FUNCTION__, GetDebugInfo(), #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPAssert_NODEBUGINFO(cond, ...) ASSERT_BEGIN do { if (!(cond)) [[unlikely]] Rendu::Assert(__FILE__, __LINE__, __FUNCTION__, "", #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPFatal(cond, ...) ASSERT_BEGIN do { if (!(cond)) [[unlikely]] Rendu::Fatal(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPError(cond, msg) ASSERT_BEGIN do { if (!(cond)) [[unlikely]] Rendu::Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPWarning(cond, msg) ASSERT_BEGIN do { if (!(cond)) [[unlikely]] Rendu::Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPAbort() ASSERT_BEGIN do { Rendu::Abort(__FILE__, __LINE__, __FUNCTION__); } while(0) ASSERT_END
#define WPAbort_MSG(msg, ...) ASSERT_BEGIN do { Rendu::Abort(__FILE__, __LINE__, __FUNCTION__, (msg), ##__VA_ARGS__); } while(0) ASSERT_END

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


#endif //RENDU_ERRORS_H