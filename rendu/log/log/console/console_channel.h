/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOG_CONSOLE_CHANNEL_H
#define RENDU_LOG_CONSOLE_CHANNEL_H

#include "log_define.h"

LOG_NAMESPACE_BEGIN

/**
 * 输出日志至终端，支持输出日志至android logcat
 */
  class ConsoleChannel : public ALogChannel {
  public:
    ConsoleChannel(const std::string &name = "ConsoleChannel", LogLevel::Level level = LogLevel::LInvalid, bool enable_color = true);

    ~ConsoleChannel() override = default;

  public:
    void SetEnableColor(bool enable_color);

    void Write(const LogContext::Ptr &ctx, ALogger &logger) override;

  protected:
    void Format(const ALogger &logger, std::ostream &ost, const LogContext::Ptr &ctx) override;


  private:
    bool enable_color_;
  };

LOG_NAMESPACE_END

#endif //RENDU_LOG_CONSOLE_CHANNEL_H
