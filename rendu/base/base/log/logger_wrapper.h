/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_LOG_LOGGER_WRAPPER_H
#define RENDU_LOG_LOGGER_WRAPPER_H

#include "base_define.h"
#include "a_logger.h"

RD_NAMESPACE_BEGIN

class LoggerWrapper {

public:
  static void Init(STRING log_flag = "default", LogLevel log_level = LogLevel::LL_TRACE,LogPosition log_position = LogPosition::LP_CONSOLE,LogMode log_mode = LogMode::LM_SYNC);
  static void SetLogger(ALogger *logger);

  static ALogger *GetLogger();

  static void SetLevel(LogLevel level);
  static LogLevel GetLevel();

  static void SetModel(LogMode mode);
  static LogMode GetModel();

  static void AddChannel(ALoggerChannel *channel);
  static void Clean();

  template<typename... Args>
  static void Write(LogMsgSource prefix, LogLevel level, Args &&...args) {
    LoggerWrapper::global_logger->Write(prefix, level, std::forward<Args>(args)...);
  }

private:
  static ALogger *global_logger;
};

#define RD_LOGGER_INIT(...) LoggerWrapper::Init(__VA_ARGS__)
#define RD_LOGGER_SET(logger) LoggerWrapper::SetLogger(logger)

#define RD_LOGGER_CLEAN() LoggerWrapper::Clean();
#define RD_LOGGER_ADD_CHANNEL(channel) LoggerWrapper::AddChannel(channel)
#define RD_LOGGER_REMOVE_CHANNEL(channel) LoggerWrapper::RemoveChannel(channel)

#define RD_LOGGER LoggerWrapper::GetLogger()

#define RD_ACTIVE_LEVEL (int) LoggerWrapper::GetLevel()

#define RD_LOG_WRITE(level, ...)                                                                \
  if ((level) < RD_ACTIVE_LEVEL) {                                                              \
    void(0);                                                                                    \
  } else {                                                                                      \
    LoggerWrapper::Write(LogMsgSource{__FILE__, __LINE__, __FUNCTION__}, (level), __VA_ARGS__); \
  }

#define RD_TRACE(...) RD_LOG_WRITE(LogLevel::LL_TRACE, __VA_ARGS__)
#define RD_DEBUG(...) RD_LOG_WRITE(LogLevel::LL_DEBUG, __VA_ARGS__)
#define RD_INFO(...) RD_LOG_WRITE(LogLevel::LL_INFO, __VA_ARGS__)
#define RD_WARN(...) RD_LOG_WRITE(LogLevel::LL_WARN, __VA_ARGS__)
#define RD_ERROR(...) RD_LOG_WRITE(LogLevel::LL_ERROR, __VA_ARGS__)
#define RD_CRITICAL(...) RD_LOG_WRITE(LogLevel::LL_CRITICAL, __VA_ARGS__)

RD_NAMESPACE_END


#endif//RENDU_LOG_LOGGER_WRAPPER_H
