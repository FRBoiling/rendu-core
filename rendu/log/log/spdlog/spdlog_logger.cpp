/*
* Created by boil on 2023/11/24.
*/

#include "spdlog_logger.h"
#include "spdlog/async.h"//support for async logging
#include "spdlog/async_logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

LOG_NAMESPACE_BEGIN

SpdlogLogger::SpdlogLogger() : ALogger("SpdLogger") {
}

void SpdlogLogger::Write(const rendu::LogContext::Ptr &ctx) {
}

void SpdlogLogger::init_console() {
  console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink_->set_pattern(console_pattern_);
  //    console_sink_->set_pattern("%+");
  sinks_.push_back(console_sink_);
}

void SpdlogLogger::init_file(const std::string &log_root_path) {
  std::string log_file_path = flag_ + ".log";
  // 创建按小时滚动的日志文件，保留一个月的日志文件
  file_sink_ = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(log_root_path + log_file_path, 0,
                                                                    24 * 30); // 24 * 30 表示一个月的天数
  file_sink_->set_pattern(file_pattern_);
  sinks_.push_back(file_sink_);
}

bool SpdlogLogger::Init(const std::string &logPath,
                  const std::string &logFlag,
                  const std::string &pattern_str,
                  LogLevel logLevel,
                  LogMode logMode,
                  LogPosition logPos
) {

  level_ = static_cast<spdlog::level::level_enum>(logLevel);
  flag_ = logFlag;
  if (!pattern_str.empty()) {
    console_pattern_ = pattern_str;
    file_pattern_ = pattern_str;
  }
  // 设置日志文件时间限制
  if (logPos == LogPosition::FILE || logPos == LogPosition::CONSOLE_AND_FILE) {
    init_file(logPath);
  }
  // 添加控制台输出
  if (logPos == LogPosition::CONSOLE || logPos == LogPosition::CONSOLE_AND_FILE) {
    init_console();
  }
  // 创建日志器
  if (logMode == LogMode::ASYNC) {
    logger_ = std::make_shared<spdlog::async_logger>(flag_, begin(sinks_), end(sinks_), spdlog::thread_pool(),
                                                     spdlog::async_overflow_policy::block);
  } else {
    logger_ = std::make_shared<spdlog::logger>(flag_, begin(sinks_), end(sinks_));
  }
  // 设置日志输出等级
  logger_->set_level(level_);
  // 设置立刻刷新日志到 disk
  logger_->flush_on(level_);
  // 每隔1秒刷新一次日志
  spdlog::flush_every(std::chrono::seconds(1));
  // 注册日志器
  spdlog::register_logger(logger_);
  return true;
}

LOG_NAMESPACE_END
