/*
* Created by boil on 2023/12/21.
*/

#include "logger_wrapper.h"

#include <utility>
#include "a_logger_channel.h"
#include "logger.h"

LOG_NAMESPACE_BEGIN
//可重置默认值
Logger *LoggerWrapper::global_logger = nullptr;

void LoggerWrapper::SetLogger(Logger *logger) {
  if (!LoggerWrapper::global_logger) {
    delete (global_logger);
    LoggerWrapper::global_logger = nullptr;
  }
  LoggerWrapper::global_logger = logger;
  if (!LoggerWrapper::global_logger) {
    LoggerWrapper::global_logger = GetLogger();
  }
}

Logger *LoggerWrapper::GetLogger() {
  if (!LoggerWrapper::global_logger) {
    auto logger = new Logger();
    logger->AddChannel(new ALoggerChannel());
    SetLogger(logger);
  }
  return LoggerWrapper::global_logger;
}

void LoggerWrapper::SetLevel(LogLevel level) {
  LoggerWrapper::global_logger->SetLevel(level);
}

LogLevel LoggerWrapper::GetLevel()  {
  return LoggerWrapper::global_logger->GetLevel();
}


void LoggerWrapper::SetModel(LogMode mode) {
  LoggerWrapper::global_logger->SetMode(mode);
}
void LoggerWrapper::Init(std::string flag/* = "default"*/,LogLevel logLevel /*= LogLevel::LL_TRACE*/) {
  GetLogger()->Init(std::move(flag),logLevel);
}

LOG_NAMESPACE_END