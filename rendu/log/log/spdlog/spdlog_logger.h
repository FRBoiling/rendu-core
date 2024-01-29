/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_BASE_LOGGER_H
#define RENDU_BASE_LOGGER_H

#include "spdlog_channel.h"
#include "logger.h"

LOG_NAMESPACE_BEGIN

class SpdLogger : public Logger {
public:
  SpdLogger(std::string flag = "spdlogger");
  ~SpdLogger() override = default;

public:
  void AddChannel(ALoggerChannel * channel) override;
  void Init(std::string flag,LogLevel logLevel) override;

private:
  [[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() const {
    return this->logger_;
  }

protected:
  void WriteMsg(LogLevel level, LogMsgSource &prefix, std::string &content) override;

private:
  std::shared_ptr<spdlog::logger> logger_;
  spdlog::level::level_enum spdlog_level_;
  std::vector<spdlog::sink_ptr> sinks_;
};

LOG_NAMESPACE_END

#endif//RENDU_BASE_LOGGER_H
