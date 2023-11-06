/*
* Created by boil on 2023/11/2.
*/

#include "logger.h"
#include "base/string/str_error.h"

RD_NAMESPACE_BEGIN

  TimeZone g_logTimeZone;

  LogLevel initLogLevel() {
      return LogLevel::TRACE;
    if (::getenv("MUDUO_LOG_TRACE"))
      return LogLevel::TRACE;
    else if (::getenv("MUDUO_LOG_DEBUG"))
      return LogLevel::DEBUG;
    else
      return LogLevel::INFO;
  }

  LogLevel g_logLevel = initLogLevel();

  LogLevel Logger::logLevel() {
    return g_logLevel;
  }

  void defaultOutput(const char *msg, int len) {
    size_t n = fwrite(msg, 1, len, stdout);
    //FIXME check n
    (void) n;
  }

  void defaultFlush() {
    fflush(stdout);
  }

  Logger::OutputFunc g_output = defaultOutput;
  Logger::FlushFunc g_flush = defaultFlush;



  Logger::Logger(SourceFile file, int line)
    : impl_(INFO, 0, file, line) {
  }

  Logger::Logger(SourceFile file, int line, LogLevel level, const char *func)
    : impl_(level, 0, file, line) {
    impl_.stream_ << func << ' ';
  }

  Logger::Logger(SourceFile file, int line, LogLevel level)
    : impl_(level, 0, file, line) {
  }

  Logger::Logger(SourceFile file, int line, bool toAbort)
    : impl_(toAbort ? FATAL : ERROR, errno, file, line) {
  }

  Logger::~Logger() {
    impl_.finish();
    const LogStream::Buffer &buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (impl_.level_ == FATAL) {
      g_flush();
      abort();
    }
  }

  void Logger::setLogLevel(LogLevel level) {
    g_logLevel = level;
  }

  void Logger::setOutput(OutputFunc out) {
    g_output = out;
  }

  void Logger::setFlush(FlushFunc flush) {
    g_flush = flush;
  }

  void Logger::setTimeZone(const TimeZone &tz) {
    g_logTimeZone = tz;
  }



RD_NAMESPACE_END