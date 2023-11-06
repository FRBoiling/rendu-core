/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_LOGGER_H
#define RENDU_COMMON_LOGGER_H

#include "impl.h"

RD_NAMESPACE_BEGIN

  class Logger {
  public:
    Logger(SourceFile file, int line);

    Logger(SourceFile file, int line, LogLevel level);

    Logger(SourceFile file, int line, LogLevel level, const char *func);

    Logger(SourceFile file, int line, bool toAbort);

    ~Logger();

  public:
    LogStream &stream() { return impl_.stream_; }

    static LogLevel logLevel();

    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char *msg, int len);

    typedef void (*FlushFunc)();

    static void setOutput(OutputFunc);

    static void setFlush(FlushFunc);

    static void setTimeZone(const TimeZone &tz);

  private:
    Impl impl_;
  };

// A small helper for CHECK_NOTNULL().
  template <typename T>
  T* CheckNotNull(SourceFile file, int line, const char *names, T* ptr)
  {
    if (ptr == NULL)
    {
      Logger(file, line, LogLevel::FATAL).stream() << names;
    }
    return ptr;
  }

#define CHECK_NOTNULL(val) \
  CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

#define LOG_TRACE if (Logger::logLevel() <= LogLevel::TRACE) \
  Logger(__FILE__, __LINE__, LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= LogLevel::DEBUG) \
  Logger(__FILE__, __LINE__, LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= LogLevel::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, LogLevel::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, LogLevel::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, LogLevel::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

RD_NAMESPACE_END

#endif //RENDU_COMMON_LOGGER_H
