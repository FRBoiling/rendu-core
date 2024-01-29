/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_LOG_LOGGER_WRAPPER_H
#define RENDU_LOG_LOGGER_WRAPPER_H

#include "logger.h"

LOG_NAMESPACE_BEGIN

class LoggerWrapper {

public:
  static void Init(std::string flag = "default", LogLevel logLevel = LogLevel::LL_TRACE);
  static void SetLogger(Logger *logger);

  static Logger *GetLogger();

  static void SetLevel(LogLevel level);
  static LogLevel GetLevel();

  static void SetModel(LogMode mode);

  template<typename... Args>
  static void Write(LogMsgSource prefix, LogLevel level, Args &&...args) {
    LoggerWrapper::global_logger->Write(prefix, level, std::forward<Args>(args)...);
  }

private:
  static Logger *global_logger;
};




#define RD_STOPWATCH spdlog::stopwatch


LOG_NAMESPACE_END

#endif//RENDU_LOG_LOGGER_WRAPPER_H
