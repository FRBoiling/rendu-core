/*
* Created by boil on 2023/9/24.
*/

#ifndef RENDU_BASE_LOGGER_H
#define RENDU_BASE_LOGGER_H

#include "spdlog_channel.h"

LOG_NAMESPACE_BEGIN

class SpdLogger : public ALogger {
public:
  SpdLogger(std::string flag = "spdlogger"): ALogger(flag){};
  ~SpdLogger() override = default;

public:
  void AddChannel(ALoggerChannel * channel) override;
  void InitChannel() override;

private:
  [[nodiscard]] std::shared_ptr<spdlog::logger> get_logger() const {
    return this->logger_;
  }

protected:
  void WriteMsg(LogLevel level, LogMsgSource prefix, const std::string &content) override;

private:
  std::shared_ptr<spdlog::logger> logger_;
  spdlog::level::level_enum spdlog_level_;
  std::vector<spdlog::sink_ptr> sinks_;
};


#define RD_STOPWATCH spdlog::stopwatch

LOG_NAMESPACE_END

#endif//RENDU_BASE_LOGGER_H
