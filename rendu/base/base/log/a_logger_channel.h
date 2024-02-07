/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_BASE_A_LOGGER_CHANNEL_H
#define RENDU_BASE_A_LOGGER_CHANNEL_H

#include "base_define.h"
#include "log_enum.h"
#include "log_msg_source.h"

RD_NAMESPACE_BEGIN

class ALogger;

class ALoggerChannel {
public:
  ALoggerChannel();
  ALoggerChannel(ALoggerChannel &&channel) = default;
  ALoggerChannel(ALoggerChannel &channel) = default;
  virtual ~ALoggerChannel() = default;

public:
  void Init(ALogger *logger);
  virtual void WriteMsg(const LogLevel level, const LogMsgSource &prefix, const std::string &content);

  string GetName() { return m_name; }

protected:
  string m_name;
  ALogger *m_logger;
};

RD_NAMESPACE_END

#endif//RENDU_BASE_A_LOGGER_CHANNEL_H
