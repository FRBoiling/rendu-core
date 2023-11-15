/*
* Created by boil on 2023/10/28.
*/

#ifndef RENDU_LOG_LOGGER_WRAPPER_H
#define RENDU_LOG_LOGGER_WRAPPER_H

#include "a_logger.h"
#include "log_context_capture.h"
#include "log_define.h"

#include "a_log_channel.h"
#include "a_log_writer.h"

RD_NAMESPACE_BEGIN

class LoggerWrapper {
public:
  template<typename First, typename... ARGS>
  static inline void
  PrintLogArray(ALogger *logger, LogLevel::Level level, const char *file, const char *function, int line, First &&first,
                ARGS &&...args) {
    LogContextCapture log(logger, level, file, function, line);
    log << std::forward<First>(first);
    AppendLog(log, std::forward<ARGS>(args)...);
  }

  static inline void PrintLogArray(ALogger *logger, LogLevel::Level level, const char *file, const char *function, int line) {
    LogContextCapture log(logger, level, file, function, line);
  }

  template<typename Log, typename First, typename... ARGS>
  static inline void AppendLog(Log &out, First &&first, ARGS &&...args) {
    out << std::forward<First>(first);
    AppendLog(out, std::forward<ARGS>(args)...);
  }

  template<typename Log>
  static inline void AppendLog(Log &out) {}

  //printf样式的日志打印
  static void
  PrintLog(ALogger *logger, int level, const char *file, const char *function, int line, const char *fmt, ...);

  static void
  PrintLogV(ALogger *logger, int level, const char *file, const char *function, int line, const char *fmt, va_list ap);
};

//可重置默认值
extern ALogger *g_defaultLogger;

extern ALogger *GetLogger();

extern void SetLogger(ALogger *logger);

#define LOG_SET_LOGGER(logger) SetLogger(logger)
#define LOG_GET_LOGGER() GetLogger()

//用法: LOG_DEBUG << 1 << "+" << 2 << '=' << 3;
#define LOG_Write(level) LogContextCapture(LOG_GET_LOGGER(), level, __FILE__, __FUNCTION__, __LINE__)
#define LOG_TRACE LOG_Write(LogLevel::LTrace)
#define LOG_DEBUG LOG_Write(LogLevel::LDebug)
#define LOG_INFO LOG_Write(LogLevel::LInfo)
#define LOG_WARN LOG_Write(LogLevel::LWarn)
#define LOG_ERROR LOG_Write(LogLevel::LError)
#define LOG_CRITICAL LOG_Write(LogLevel::LCritical)

//只能在虚继承BaseLogFlagInterface的类中使用
#define WriteF(level) LogContextCapture(LOG_GET_LOGGER(), level, __FILE__, __FUNCTION__, __LINE__, getLogFlag())
#define TraceF WriteF(LTrace)
#define DebugF WriteF(LDebug)
#define InfoF WriteF(LInfo)
#define WarnF WriteF(LWarn)
#define ErrorF WriteF(LError)

//用法: PrintD("%d + %s = %c", 1 "2", 'c');
#define PrintL(level, ...) LoggerWrapper::PrintLog(LOG_GET_LOGGER(), level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define PrintT(...) PrintL(LogLevel::LTrace, ##__VA_ARGS__)
#define PrintD(...) PrintL(LogLevel::LDebug, ##__VA_ARGS__)
#define PrintI(...) PrintL(LogLevel::LInfo, ##__VA_ARGS__)
#define PrintW(...) PrintL(LogLevel::LWarn, ##__VA_ARGS__)
#define PrintE(...) PrintL(LogLevel::LError, ##__VA_ARGS__)
#define PrintC(...) PrintL(LogLevel::LCritical, ##__VA_ARGS__)

//用法: LogD(1, "+", "2", '=', 3);
//用于模板实例化的原因，如果每次打印参数个数和类型不一致，可能会导致二进制代码膨胀
#define LogL(level, ...) LoggerWrapper::PrintLogArray(LOG_GET_LOGGER(), (LogLevel::Level) level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LogT(...) LogL(LogLevel::LTrace, ##__VA_ARGS__)
#define LogD(...) LogL(LogLevel::LDebug, ##__VA_ARGS__)
#define LogI(...) LogL(LogLevel::LInfo, ##__VA_ARGS__)
#define LogW(...) LogL(LogLevel::LWarn, ##__VA_ARGS__)
#define LogE(...) LogL(LogLevel::LError, ##__VA_ARGS__)
#define LogC(...) LogL(LogLevel::LCritical, ##__VA_ARGS__)


RD_NAMESPACE_END

#endif//RENDU_LOG_LOGGER_WRAPPER_H
