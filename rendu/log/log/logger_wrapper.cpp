/*
* Created by boil on 2023/10/28.
*/

#include "logger_wrapper.h"
LOG_NAMESPACE_BEGIN

void LoggerWrapper::printLogV(ALogger &logger, int level, const char *file, const char *function, int line, const char *fmt,
                              va_list ap) {
  LogContextCapture info(&logger, (LogLevel::Level) level, file, function, line);
  char *str = nullptr;
  if (vasprintf(&str, fmt, ap) > 0 && str) {
    info << str;
    free(str);
  }
}

void LoggerWrapper::printLog(ALogger &logger, int level, const char *file, const char *function, int line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  printLogV(logger, level, file, function, line, fmt, ap);
  va_end(ap);
}

LOG_NAMESPACE_END