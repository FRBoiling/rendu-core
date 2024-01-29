/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_LOG_H
#define RENDU_LOG_H

#include "logger_wrapper.h"

LOG_NAMESPACE_BEGIN

#define RD_LOGGER_INIT(...) LoggerWrapper::Init(__VA_ARGS__)
#define RD_LOGGER_SET(...) LoggerWrapper::SetLogger(__VA_ARGS__)

#define RD_LOGGER_CLEAN() RD_LOGGER->Clean();
#define RD_LOGGER_ADD_CHANNEL(...) RD_LOGGER->AddChannel(__VA_ARGS__)

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

LOG_NAMESPACE_END

#include "default/console_channel.h"

#include "spdlog/spdlog_console_channel.h"
#include "spdlog/spdlog_file_channel.h"
#include "spdlog/spdlog_logger.h"

#endif//RENDU_LOG_H
