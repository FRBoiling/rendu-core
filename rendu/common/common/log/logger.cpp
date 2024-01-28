/*
* Created by boil on 2023/12/21.
*/

#include "logger.h"

#include <utility>

COMMON_NAMESPACE_BEGIN

Logger::Logger() : level_(LogLevel::LL_TRACE), mode_(LogMode::LM_SYNC) {
}

void Logger::Init(std::string flag/* = "default"*/,LogLevel logLevel /*= LogLevel::LL_TRACE*/) {
  flag_ = flag;
  level_ = logLevel;
}

void Logger::AddChannel(ALoggerChannel *channel) {
  channel->Init(this);
  channels_.push_back(std::move(channel));
}

void Logger::SetLevel(LogLevel log_level) {
  level_ = log_level;
}

const LogLevel Logger::GetLevel() const{
  return level_;
}

void Logger::SetMode(LogMode log_mode) {
  mode_ = log_mode;
}

void Logger::SetFlag(const std::string &flag) {
  flag_ = flag;
}
const std::string &Logger::GetFlag() const {
  return flag_;
}

COMMON_NAMESPACE_END