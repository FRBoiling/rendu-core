/*
* Created by boil on 2023/10/28.
*/

#include "logger_wrapper.h"

RD_NAMESPACE_BEGIN

void LoggerWrapper::PrintLogV(ALogger *logger, int level, const char *file, const char *function, int line, const char *fmt,
                              va_list ap) {
  LogContextCapture info(logger, (LogLevel::Level) level, file, function, line);
  char *str = nullptr;
  if (vasprintf(&str, fmt, ap) > 0 && str) {
    info << str;
    free(str);
  }
}

void LoggerWrapper::PrintLog(ALogger *logger, int level, const char *file, const char *function, int line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  PrintLogV(logger, level, file, function, line, fmt, ap);
  va_end(ap);
}

//可重置默认值
ALogger *g_defaultLogger = nullptr;

ALogger *GetLogger() {
  if (!g_defaultLogger) {
    g_defaultLogger = new ALogger("default");
    //  logger->Add(std::make_shared<ConsoleChannel>());
    //  logger->SetWriter(std::make_shared<AsyncLogWriter>());
    //  logger->SetLevel(LogLevel::LTrace);

    g_defaultLogger->Add(std::make_shared<ALogChannel>());
    g_defaultLogger->SetWriter(std::make_shared<ALogWriter>());
    g_defaultLogger->SetLevel(LogLevel::LTrace);
  }
  return g_defaultLogger;
}

void SetLogger(ALogger* logger) {
  g_defaultLogger = logger;
}

RD_NAMESPACE_END