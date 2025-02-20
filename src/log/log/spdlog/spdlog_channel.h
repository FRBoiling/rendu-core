/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_SPDLOG_CHANNEL_H
#define RENDU_SPDLOG_CHANNEL_H

#include "log_define.h"

#include <spdlog/fmt/ostr.h>// support for user defined types
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/hourly_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <utility>

LOG_NAMESPACE_BEGIN

#define FILE_MSG_PATTERN "%Y-%m-%d %H:%M:%S.%e[%n][%l][%t][%s:%#]$ %v"
#define CONSOLE_MSG_PATTERN "%^%Y-%m-%d %H:%M:%S.%e[%n][%L][%t][%!,%s:%#]$ %v%$"

class SpdlogChannel : public ALoggerChannel {
public:
  SpdlogChannel(std::string msg_pattern) : msg_pattern_(std::move(msg_pattern)){};
  virtual ~SpdlogChannel() = default;

public:
  spdlog::sink_ptr GetSinkPtr() {
    return sink_ptr_;
  }

protected:
  spdlog::sink_ptr sink_ptr_;
  std::string msg_pattern_{"[%Y-%m-%d %H:%M:%S.%e][%n][%l][thread:%t][%!,%s:%#] %v"};

};

LOG_NAMESPACE_END

#endif//RENDU_SPDLOG_CHANNEL_H
