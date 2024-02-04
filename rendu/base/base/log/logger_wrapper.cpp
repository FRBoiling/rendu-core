/*
* Created by boil on 2023/12/21.
*/

#include "logger_wrapper.h"

#include <utility>

RD_NAMESPACE_BEGIN
//可重置默认值
ALogger *LoggerWrapper::global_logger = nullptr;

void LoggerWrapper::SetLogger(ALogger *logger) {
  if (!LoggerWrapper::global_logger) {
    delete (global_logger);
    LoggerWrapper::global_logger = nullptr;
  }
  LoggerWrapper::global_logger = logger;
  if (!LoggerWrapper::global_logger) {
    LoggerWrapper::global_logger = GetLogger();
  }
}

ALogger *LoggerWrapper::GetLogger() {
  if (!LoggerWrapper::global_logger) {
    auto logger = new ALogger("default");
    SetLogger(logger);
  }
  return LoggerWrapper::global_logger;
}

void LoggerWrapper::SetLevel(LogLevel level) {
  GetLogger()->SetLevel(level);
}

LogLevel LoggerWrapper::GetLevel()  {
  return GetLogger()->GetLevel();
}


void LoggerWrapper::SetModel(LogMode mode) {
  GetLogger()->SetMode(mode);
}

LogMode LoggerWrapper::GetModel() {
  return GetLogger()->GetMode();
}

void LoggerWrapper::AddChannel(ALoggerChannel *channel) {
  GetLogger()->AddChannel(channel);
}

void LoggerWrapper::Clean() {
  GetLogger()->Clean();
  delete (LoggerWrapper::global_logger);
  LoggerWrapper::global_logger = nullptr;
}


void LoggerWrapper::Init(std::string log_flag/* = "default"*/,LogLevel log_level /*= LogLevel::LL_TRACE*/,LogPosition log_position/* = LogPosition::LP_CONSOLE*/,LogMode log_mode /*= LogMode::LM_ASYNC*/) {
  GetLogger()->Init(log_flag,log_level,log_position,log_mode);
}




RD_NAMESPACE_END