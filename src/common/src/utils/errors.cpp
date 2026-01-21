//
// Created by 沸腾 on 2025/11/22.
//

#include "common/utils/errors.h"
#include <cstdio>
#include <thread>
#include <cstdarg>

#include "common/utils/string_format.h"

/**
    @file Errors.cpp

    @brief 该文件包含用于报告关键应用程序错误的函数定义

    非常重要的一点是：永远不要用abort()替代*((volatile int*)nullptr) = 0;
    在Windows上调用abort()不会调用未处理异常过滤器 - 这是WheatyExceptionReport用于记录崩溃的机制。
    这里的exit(1)调用是为了静态分析工具，表明调用此文件中定义的函数会终止应用程序。
 */

#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
#include <Windows.h>
#include <intrin.h>
// Windows平台崩溃宏：通过抛出异常来触发崩溃处理
#define Crash(message) \
    ULONG_PTR execeptionArgs[] = { reinterpret_cast<ULONG_PTR>(strdup(message)), reinterpret_cast<ULONG_PTR>(_ReturnAddress()) }; \
    RaiseException(EXCEPTION_ASSERTION_FAILURE, 0, 2, execeptionArgs);
#else
#include <cstring>

// 在gdb中应该容易访问的全局变量
extern "C" {
RC_COMMON_API char const* TrinityAssertionFailedMessage = nullptr;
}

// Unix/Linux平台崩溃宏：通过空指针解引用来触发崩溃
#define Crash(message) \
    TrinityAssertionFailedMessage = strdup(message); \
    *((volatile int*)nullptr) = 0; \
    exit(1);
#endif

namespace
{
    /**
     * @brief 格式化断言消息的辅助函数
     * @param format 格式化字符串
     * @param args 可变参数列表
     * @return 格式化后的字符串
     */
    std::string FormatAssertionMessage(char const* format, va_list args)
    {
        std::string formatted;
        va_list len;

        // 计算格式化字符串的长度
        va_copy(len, args);
        int32 length = vsnprintf(nullptr, 0, format, len);
        va_end(len);

        // 调整字符串大小并执行格式化
        formatted.resize(length);
        vsnprintf(&formatted[0], length + 1, format, args);

        return formatted;
    }
}

BEGIN_NAMESPACE_COMMON
    /**
     * @brief 基本断言函数 - 当条件不满足时终止程序
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param debugInfo 调试信息
     * @param message 断言消息
     */
    void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message)
    {
        // 格式化错误消息，包含文件、行号、函数名和断言消息
        std::string formattedMessage = Utils::StringFormat("\n{}:{} in {} ASSERTION FAILED:\n  {}\n", file, line, function,
                                                           message) + debugInfo + '\n';
        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);
        Crash(formattedMessage.c_str());
    }

    /**
     * @brief 带格式化参数的断言函数
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param debugInfo 调试信息
     * @param message 断言消息
     * @param format 格式化字符串
     * @param ... 可变参数
     */
    void Assert(char const* file, int line, char const* function, std::string debugInfo, char const* message,
                char const* format, ...)
    {
        va_list args;
        va_start(args, format);

        // 格式化包含额外参数的错误消息
        std::string formattedMessage = Utils::StringFormat("\n{}:{} in {} ASSERTION FAILED:\n  {}\n", file, line, function,
                                                           message) + FormatAssertionMessage(format, args) + '\n' + debugInfo +
            '\n';
        va_end(args);

        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);

        Crash(formattedMessage.c_str());
    }

    /**
     * @brief 致命错误函数 - 用于不可恢复的错误
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param message 错误消息
     * @param ... 可变参数
     */
    void Fatal(char const* file, int line, char const* function, char const* message, ...)
    {
        va_list args;
        va_start(args, message);

        // 格式化致命错误消息
        std::string formattedMessage = Utils::StringFormat("\n{}:{} in {} FATAL ERROR:\n", file, line, function) +
            FormatAssertionMessage(message, args) + '\n';
        va_end(args);

        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);

        // 等待10秒以便查看错误信息
        std::this_thread::sleep_for(std::chrono::seconds(10));
        Crash(formattedMessage.c_str());
    }

    /**
     * @brief 错误函数 - 用于一般性错误
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param message 错误消息
     */
    void Error(char const* file, int line, char const* function, char const* message)
    {
        std::string formattedMessage = Utils::StringFormat("\n{}:{} in {} ERROR:\n  {}\n", file, line, function, message);
        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);
        Crash(formattedMessage.c_str());
    }

    /**
     * @brief 警告函数 - 用于非致命警告信息
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param message 警告消息
     */
    void Warning(char const* file, int line, char const* function, char const* message)
    {
        fprintf(stderr, "\n%s:%i in %s WARNING:\n  %s\n",
                file, line, function, message);
    }

    /**
     * @brief 中止函数 - 无条件终止程序
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     */
    void Abort(char const* file, int line, char const* function)
    {
        std::string formattedMessage = Utils::StringFormat("\n{}:{} in {} ABORTED.\n", file, line, function);
        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);
        Crash(formattedMessage.c_str());
    }

    /**
     * @brief 带消息的中止函数
     * @param file 文件名
     * @param line 行号
     * @param function 函数名
     * @param message 中止消息
     * @param ... 可变参数
     */
    void Abort(char const* file, int line, char const* function, char const* message, ...)
    {
        va_list args;
        va_start(args, message);

        std::string formattedMessage = Utils::StringFormat("\n{}:{} in {} ABORTED:\n", file, line, function) +
            FormatAssertionMessage(message, args) + '\n';
        va_end(args);

        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);

        Crash(formattedMessage.c_str());
    }

    /**
     * @brief 信号处理函数 - 用于处理程序信号
     * @param sigval 信号值
     */
    void AbortHandler(int sigval)
    {
        // 这里没有有用的信息可以记录，无法传递参数
        std::string formattedMessage = Utils::StringFormat("Caught signal {}\n", sigval);
        fprintf(stderr, "%s", formattedMessage.c_str());
        fflush(stderr);
        Crash(formattedMessage.c_str());
    }

END_NAMESPACE_COMMON

/**
 * @brief 获取调试信息（当前为空实现）
 * @return 空字符串
 */
std::string GetDebugInfo()
{
    return "";
}
