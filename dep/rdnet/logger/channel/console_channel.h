/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_CONSOLE_CHANNEL_H
#define RENDU_CONSOLE_CHANNEL_H

#include "a_log_channel.h"

RD_NAMESPACE_BEGIN

/**
 * 输出日志至终端，支持输出日志至android logcat
 */
  class ConsoleChannel : public ALogChannel {
  public:
    ConsoleChannel(const std::string &name = "ConsoleChannel", LogLevel level = LTrace);

    ~ConsoleChannel() override = default;

    void write(const Logger &logger, const LogContext::Ptr &logContext) override;
  };

RD_NAMESPACE_END

#endif //RENDU_CONSOLE_CHANNEL_H
