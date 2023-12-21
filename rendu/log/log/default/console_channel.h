/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_LOG_CONSOLE_CHANNEL_H
#define RENDU_LOG_CONSOLE_CHANNEL_H

#include "log_define.h"

LOG_NAMESPACE_BEGIN

using namespace common;
class ConsoleChannel : public ALoggerChannel {
public:
  explicit ConsoleChannel();
  virtual ~ConsoleChannel() = default;

public:
  void WriteMsg(LogLevel level, LogMsgSource &prefix, std::string &content) override;

protected:

};

LOG_NAMESPACE_END

#endif//RENDU_LOG_CONSOLE_CHANNEL_H
