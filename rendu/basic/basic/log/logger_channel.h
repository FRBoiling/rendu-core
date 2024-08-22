/*
* Created by boil on 2024/8/9.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOGGER_CHANNEL_H_
#define RENDU_BASIC_BASIC_LOG_LOGGER_CHANNEL_H_

#include <utility>

#include "log_level.h"
#include "log_msg.h"
#include "log_msg_source.h"

RD_NAMESPACE_BEGIN
class Logger;

class LoggerChannel {

public:
  LoggerChannel(Logger *p_logger,String name = "default channel");
  virtual ~LoggerChannel() = default;

public:
  String getName() const { return m_name; };
  void setName(String name) { m_name = std::move(name); };

  void write(LogLevel level, LogMsgSource &prefix, const LogMsg &content);

protected:
  String m_name;
  Logger *m_logger;
};

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_LOG_LOGGER_CHANNEL_H_
