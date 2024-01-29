/*
* Created by boil on 2023/12/21.
*/

#include "spdlog_file_channel.h"
#include "logger.h"

LOG_NAMESPACE_BEGIN

SpdLogFileChannel::SpdLogFileChannel(Logger *logger) : SpdlogChannel(FILE_MSG_PATTERN) {
  std::string log_file_path = logger->GetFlag() + ".log";
  std::string log_root_path = "../log";
  // 创建按小时滚动的日志文件，保留一个月的日志文件
  sink_ptr_ = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(log_root_path + log_file_path, 0,
                                                                   24 * 30);// 24 * 30 表示一个月的天数
  sink_ptr_->set_pattern(msg_pattern_);
}

LOG_NAMESPACE_END