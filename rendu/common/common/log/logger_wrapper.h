/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_COMMON_LOGGER_WRAPPER_H
#define RENDU_COMMON_LOGGER_WRAPPER_H

#include "logger.h"

COMMON_NAMESPACE_BEGIN

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

#define RD_LOGGER_INIT(...) LoggerWrapper::Init(__VA_ARGS__)
#define RD_LOGGER_SET(...) LoggerWrapper::SetLogger(__VA_ARGS__)

#define RD_LOGGER_CLEAN() RD_LOGGER->Clean();
#define RD_LOGGER_ADD_CHANNEL(...) RD_LOGGER->AddChannel(__VA_ARGS__)

#define RD_LOGGER LoggerWrapper::GetLogger()

#define RD_ACTIVE_LEVEL (int)LoggerWrapper::GetLevel()

#define RD_LOG_WRITE(level, ...)   \
  if((level) < RD_ACTIVE_LEVEL) { \
    void(0); \
  } else { \
    LoggerWrapper::Write(LogMsgSource{__FILE__, __LINE__, __FUNCTION__}, (level), __VA_ARGS__); \
  }

#define RD_TRACE(...) RD_LOG_WRITE(LogLevel::LL_TRACE, __VA_ARGS__)
#define RD_DEBUG(...) RD_LOG_WRITE(LogLevel::LL_DEBUG, __VA_ARGS__)
#define RD_INFO(...) RD_LOG_WRITE(LogLevel::LL_INFO, __VA_ARGS__)
#define RD_WARN(...) RD_LOG_WRITE(LogLevel::LL_WARN, __VA_ARGS__)
#define RD_ERROR(...) RD_LOG_WRITE(LogLevel::LL_ERROR, __VA_ARGS__)
#define RD_CRITICAL(...) RD_LOG_WRITE(LogLevel::LL_CRITICAL, __VA_ARGS__)


#define RD_STOPWATCH spdlog::stopwatch


COMMON_NAMESPACE_END

#endif//RENDU_COMMON_LOGGER_WRAPPER_H
