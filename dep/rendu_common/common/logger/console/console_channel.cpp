/*
* Created by boil on 2023/10/26.
*/

#include "console_channel.h"
#include "log_color.h"
#include <iostream>

RD_NAMESPACE_BEGIN

  ConsoleChannel::ConsoleChannel(const string &name, LogLevel::Level level, bool enable_color) : ALogChannel(name, level),
                                                                                          enable_color_(enable_color) {}

  void ConsoleChannel::SetEnableColor(bool enable_color) {
    enable_color_ = enable_color;
  }

  void ConsoleChannel::Format(const Logger &logger, std::ostream &ost, const LogContext::Ptr &ctx) {
    if (enable_color_) {
      // color console start
#ifdef _WIN32
      SetConsoleColor(LOG_CONST_TABLE[ctx->_level][1]);
#else
      ost << LogColor::SET_COLOR(ctx->GetLogLevel());
#endif
    }

    ALogChannel::Format(logger, ost, ctx);

    if (enable_color_) {
      // color console end
#ifdef _WIN32
      SetConsoleColor(CLEAR_COLOR);
#else
      ost << CLEAR_COLOR;
#endif
    }

  }

  void ConsoleChannel::Write(const Logger &logger, const LogContext::Ptr &ctx) {
    if (_level > ctx->GetLogLevel()) {
      return;
    }

#if defined(OS_IPHONE)
    //ios禁用日志颜色
    format(logger, std::cout, ctx, false);
#elif defined(ANDROID)
    static android_LogPriority LogPriorityArr[10];
    static onceToken s_token([](){
        LogPriorityArr[LTrace] = ANDROID_LOG_VERBOSE;
        LogPriorityArr[LDebug] = ANDROID_LOG_DEBUG;
        LogPriorityArr[LInfo] = ANDROID_LOG_INFO;
        LogPriorityArr[LWarn] = ANDROID_LOG_WARN;
        LogPriorityArr[LError] = ANDROID_LOG_ERROR;
    });
    __android_log_print(LogPriorityArr[ctx->_level],"JNI","%s %s",ctx->_function.data(),ctx->str().data());
#else
    //linux/windows日志启用颜色并显示日志详情
    Format(logger, std::cout, ctx);
#endif
  }


RD_NAMESPACE_END