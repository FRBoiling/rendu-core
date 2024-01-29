/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_LOG_A_LOGGER_CHANNEL_H
#define RENDU_LOG_A_LOGGER_CHANNEL_H

#include "log_enum.h"
#include "log_msg.h"

LOG_NAMESPACE_BEGIN

class Logger ;

class ALoggerChannel {
public:
  ALoggerChannel();
  ALoggerChannel(ALoggerChannel&& channel) = default;
  ALoggerChannel(ALoggerChannel& channel) = default;
  virtual ~ALoggerChannel() = default;
public:
  void Init(Logger* logger);

protected:
  Logger *logger_;

public:
  virtual void WriteMsg(LogLevel level, LogMsgSource & prefix, std::string & content);
};

LOG_NAMESPACE_END

#endif//RENDU_LOG_A_LOGGER_CHANNEL_H
