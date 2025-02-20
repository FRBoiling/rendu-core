/*
* Created by boil on 2023/12/21.
*/

#include "spdlog_console_channel.h"

LOG_NAMESPACE_BEGIN


SpdLogConsoleChannel::SpdLogConsoleChannel() : SpdlogChannel(CONSOLE_MSG_PATTERN) {
  sink_ptr_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  sink_ptr_->set_pattern(msg_pattern_);
}

LOG_NAMESPACE_END