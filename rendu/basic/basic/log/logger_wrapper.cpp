/*
* Created by boil on 2024/8/22.
*/

#include "logger_wrapper.h"
RD_NAMESPACE_BEGIN
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
    auto logger = new Logger("default");
    SetLogger(logger);
  }
  return LoggerWrapper::global_logger;
}

void LoggerWrapper::SetLevel(LogLevel level) {
  GetLogger()->setLevel(level);
}

LogLevel LoggerWrapper::GetLevel()  {
  return GetLogger()->getLevel();
}


void LoggerWrapper::SetModel(LogMode mode) {
  GetLogger()->setMode(mode);
}

LogMode LoggerWrapper::GetModel() {
  return GetLogger()->getMode();
}

void LoggerWrapper::AddChannel(LoggerChannel *channel) {
  GetLogger()->addChannel(channel);
}

void LoggerWrapper::Clean() {
  GetLogger()->clean();
  delete (LoggerWrapper::global_logger);
  LoggerWrapper::global_logger = nullptr;
}


void LoggerWrapper::Init(std::string log_flag/* = "default"*/,LogLevel log_level /*= LogLevel::LL_TRACE*/,LogPosition log_position/* = LogPosition::LP_CONSOLE*/,LogMode log_mode /*= LogMode::LM_ASYNC*/) {
  GetLogger()->init(log_flag,log_level,log_position,log_mode);
}

RD_NAMESPACE_END