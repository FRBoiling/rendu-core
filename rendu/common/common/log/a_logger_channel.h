/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_COMMON_A_LOGGER_CHANNEL_H
#define RENDU_COMMON_A_LOGGER_CHANNEL_H

#include "logger_enum.h"
#include "log_msg.h"

COMMON_NAMESPACE_BEGIN

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

COMMON_NAMESPACE_END

#endif//RENDU_COMMON_A_LOGGER_CHANNEL_H
