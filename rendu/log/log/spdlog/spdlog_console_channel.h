/*
* Created by boil on 2023/11/24.
*/

#ifndef RENDU_SPD_CHANNEL_H
#define RENDU_SPD_CHANNEL_H

#include "log_define.h"

LOG_NAMESPACE_BEGIN

class SpdlogConsoleChannel : public ALogChannel {

public:
  SpdlogConsoleChannel(const std::string &name = "SpdlogConsoleChannel", LogLevel::Level level = LogLevel::LInvalid, bool enable_color = true);

  ~SpdlogConsoleChannel() override = default;

public:
  void Write(const LogContext::Ptr &ctx, ALogger &logger) override;

protected:
  void Format(const ALogger &logger, std::ostream &ost, const LogContext::Ptr &ctx) override;

private:
  std::string console_pattern_{"[%m-%d %H:%M:%S.%e][%n][%^%L%$] [%s:%#] %v"};

  bool enable_color_;
};

LOG_NAMESPACE_END

#endif//RENDU_SPD_CHANNEL_H
