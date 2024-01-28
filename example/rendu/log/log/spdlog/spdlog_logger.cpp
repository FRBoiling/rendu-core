/*
* Created by boil on 2023/9/24.
*/

#include "spdlog_logger.h"
#include "spdlog/async.h"//support for async logging
#include "spdlog/async_logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

LOG_NAMESPACE_BEGIN

SpdLogger::SpdLogger(std::string flag) {
}

void SpdLogger::Init(std::string flag /*= "spdlogger"*/, LogLevel logLevel /*= LogLevel::LL_TRACE*/) {
  Logger::Init(flag,logLevel);
  spdlog_level_ = static_cast<spdlog::level::level_enum>(level_);
  if (mode_ == LogMode::LM_ASYNC) {
    logger_ = std::make_shared<spdlog::async_logger>(flag_, begin(sinks_), end(sinks_), spdlog::thread_pool(),
                                                     spdlog::async_overflow_policy::block);
  } else {
    logger_ = std::make_shared<spdlog::logger>(flag_, begin(sinks_), end(sinks_));
  }
  // 设置日志输出等级
  logger_->set_level(spdlog_level_);
  // 设置立刻刷新日志到 disk
  logger_->flush_on(spdlog_level_);
  // 每隔1秒刷新一次日志
  spdlog::flush_every(std::chrono::seconds(1));
  // 注册日志器
  spdlog::register_logger(logger_);
}

void SpdLogger::AddChannel(ALoggerChannel *channel) {
  auto spdlog_channel = (SpdlogChannel *) (channel);
  sinks_.push_back(spdlog_channel->GetSinkPtr());
}

void SpdLogger::WriteMsg(LogLevel level, LogMsgSource &prefix, std::string &content) {
  logger_->log(spdlog::source_loc{prefix.GetFileName(), prefix.GetLine(), prefix.GetFunctionName()}, static_cast<spdlog::level::level_enum>(level), content);
}

LOG_NAMESPACE_END