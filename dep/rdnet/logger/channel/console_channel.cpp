/*
* Created by boil on 2023/10/26.
*/

#include "console_channel.h"
#include <iostream>

RD_NAMESPACE_BEGIN

  ConsoleChannel::ConsoleChannel(const string &name, LogLevel level) : ALogChannel(name, level) {}

  void ConsoleChannel::write(const Logger &logger, const LogContext::Ptr &ctx) {
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