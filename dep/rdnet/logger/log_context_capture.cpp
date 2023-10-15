/*
* Created by boil on 2023/10/26.
*/

#include "log_context_capture.h"
#include <iostream>
#include "path/path.h"
#include "logger.h"

RD_NAMESPACE_BEGIN

  static string s_module_name = PathHelper::exeName(false);

  LogContextCapture::LogContextCapture(Logger &logger, LogLevel level, const char *file, const char *function, int line, const char *flag) :
    _logger_ctx(new LogContext(level, file, function, line, s_module_name.c_str(), flag)), _logger(logger) {
  }

  LogContextCapture::LogContextCapture(const LogContextCapture &that) : _logger_ctx(that._logger_ctx), _logger(that._logger) {
    const_cast<LogContext::Ptr &>(that._logger_ctx).reset();
  }

  LogContextCapture::~LogContextCapture() {
    *this << std::endl;
  }

  void LogContextCapture::Clear() {
    _logger_ctx.reset();
  }

  LogContextCapture &LogContextCapture::operator<<(std::ostream &(*f)(std::ostream &)) {
    if (!_logger_ctx) {
      return *this;
    }
    _logger.Write(_logger_ctx);
    _logger_ctx.reset();
    return *this;
  }

RD_NAMESPACE_END