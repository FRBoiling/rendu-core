/*
* Created by boil on 2023/9/24.
*/

#include "spdlog_logger.h"
#include "spdlog/async.h"//support for async logging
#include "spdlog/async_logger.h"
#include "spdlog_console_channel.h"
#include "spdlog_file_channel.h"

LOG_NAMESPACE_BEGIN

void SpdLogger::InitChannel() {
  spdlog_level_ = static_cast<spdlog::level::level_enum>(GetLevel());

  if (GetPosition() == LogPosition::LP_CONSOLE || GetPosition() == LogPosition::LP_CONSOLE_AND_FILE) {
    AddChannel(new SpdLogConsoleChannel());
  }
  if (GetPosition() == LogPosition::LP_FILE || GetPosition() == LogPosition::LP_CONSOLE_AND_FILE) {
    AddChannel(new SpdLogFileChannel());
  }

  if (GetMode() == LogMode::LM_ASYNC) {
    logger_ = std::make_shared<spdlog::async_logger>(GetFlag(), begin(sinks_), end(sinks_), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
  } else {
    logger_ = std::make_shared<spdlog::logger>(GetFlag(), begin(sinks_), end(sinks_));
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

void SpdLogger::WriteMsg(LogLevel level, LogMsgSource prefix, const std::string &content) {
  logger_->log(spdlog::source_loc{prefix.GetFileName(), prefix.GetLine(), prefix.GetFunctionName()}, static_cast<spdlog::level::level_enum>(level), content);
  ALogger::WriteMsg(level, prefix, content);
}

LOG_NAMESPACE_END