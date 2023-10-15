/*
* Created by boil on 2023/10/28.
*/

#include "logger_wrapper.h"

RD_NAMESPACE_BEGIN

  void
  LoggerWrapper::printLogV(Logger &logger, int level, const char *file, const char *function, int line, const char *fmt,
                           va_list ap) {
    LogContextCapture info(logger, (LogLevel) level, file, function, line);
    char *str = nullptr;
    if (vasprintf(&str, fmt, ap) > 0 && str) {
      info << str;
      free(str);
    }
  }

  void
  LoggerWrapper::printLog(Logger &logger, int level, const char *file, const char *function, int line, const char *fmt,
                          ...) {
    va_list ap;
    va_start(ap, fmt);
    printLogV(logger, level, file, function, line, fmt, ap);
    va_end(ap);
  }

RD_NAMESPACE_END