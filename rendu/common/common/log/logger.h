/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_COMMON_A_LOGGER_H
#define RENDU_COMMON_A_LOGGER_H

#include "a_logger_channel.h"
#include "log_msg_source.h"
#include "logger_enum.h"
#include <iostream>

COMMON_NAMESPACE_BEGIN

class Logger {
public:
  Logger();
  virtual ~Logger() = default;

public:
  virtual void Init(std::string flag,LogLevel logLevel);
  virtual void AddChannel(ALoggerChannel *channel);

public:
  void SetLevel(LogLevel log_level);
  void SetMode(LogMode log_mode);
  void SetFlag(const std::string &flag);
  const std::string &GetFlag() const;
  const LogLevel GetLevel() const;

  void Clean(){
    channels_.clear();
  }
protected:
  LogLevel level_;
  LogMode mode_;
  std::vector<ALoggerChannel *> channels_;
  std::string flag_;

public:
  template<typename... Args>
  void Write(LogMsgSource prefix, LogLevel level, Args &&...args) {
    auto msg_content = WriteContent(std::forward<Args>(args)...);
    WriteMsg(level, prefix, msg_content);
  }

protected:
  template<typename... Args>
  std::string WriteContent(string_view_t fmt, Args &&...args) {
    auto ttt = fmt::make_format_args(args...);
    memory_buf_t buf;
    fmt::detail::vformat_to(buf, fmt, fmt::make_format_args(args...));
    auto content = std::string{buf.data(), buf.size()};
    return content;
  }

  virtual void WriteMsg(LogLevel level, LogMsgSource &prefix, std::string &content) {
    for (auto &channel: channels_) {
      channel->WriteMsg(level, prefix, content);
    }
  };
};

COMMON_NAMESPACE_END

#endif//RENDU_COMMON_A_LOGGER_H
