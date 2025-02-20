/*
* Created by boil on 2024/8/9.
*/

#ifndef RENDU_BASIC_BASIC_LOG_LOGGER_H_
#define RENDU_BASIC_BASIC_LOG_LOGGER_H_

#include <utility>

#include "log_level.h"
#include "log_mode.h"
#include "log_msg.h"
#include "log_msg_source.h"
#include "log_position.h"
#include "logger_channel.h"

RD_NAMESPACE_BEGIN

class Logger {
public:
  Logger(String flag, LogMode mode = LogMode::LM_SYNC, LogPosition position = LogPosition::LP_CONSOLE, LogLevel level = LogLevel::LL_TRACE)
      : flag_(std::move(flag)), level_(level), position_(position), mode_(mode) {}
  virtual ~Logger() = default;

protected:
  String flag_;
  LogLevel level_;
  LogPosition position_;
  LogMode mode_;
  std::vector<LoggerChannel *> channels_;

public:
  template<typename... Args>
  void write(LogMsgSource prefix, LogLevel level, Args &&...args) {
    setLevel(level);
    write(prefix, writeContent(args...));
  }

protected:
  template<typename... Args>
  LogMsg writeContent(FormatString<Args...> format_string, Args &&... args)
  {
    LogMsg msg_;
    msg_.setMsg(format_string, args...);
    return msg_;
  }

  template<typename T>
  LogMsg writeContent(const T &msg)
  {
    LogMsg msg_;
    msg_.setMsg(msg);
    return msg_;
  }

  virtual void write(LogMsgSource prefix, LogMsg content) ;

public:
  virtual void init(String flag, LogLevel log_level, LogPosition log_position, LogMode log_mode);

  virtual void addChannel(LoggerChannel *channel);

  void setFlag(String flag);
  String getFlag() const;

  void setLevel(LogLevel level);
  LogLevel getLevel() const;

  void setMode(LogMode mode);
  LogMode getMode() const;

  void setPosition(LogPosition position);
  LogPosition getPosition() const;

  void clean();


};




RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_LOG_LOGGER_H_
