/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_BASE_BASE_LOG_A_LOGGER_H_
#define RENDU_BASE_BASE_LOG_A_LOGGER_H_

#include "a_logger_channel.h"
#include "base_define.h"

RD_NAMESPACE_BEGIN

class ALogger {
public:
  ALogger(STRING flag, LogMode mode = LogMode::LM_SYNC, LogPosition position = LogPosition::LP_CONSOLE, LogLevel level = LogLevel::LL_TRACE) : flag_(flag), level_(level), position_(position), mode_(mode) {}
  virtual ~ALogger() = default;

protected:
  STRING flag_;
  LogLevel level_;
  LogPosition position_;
  LogMode mode_;
  std::vector<ALoggerChannel *> channels_;

public:
  template<typename... Args>
  void Write(LogMsgSource prefix, LogLevel level, Args &&...args) {
    auto msg_content = WriteContent(std::forward<Args>(args)...);
    WriteMsg(level, prefix, msg_content);
  }

protected:
  template<typename... Args>
  std::string WriteContent(string_view_t fmt, Args &&...args) {
    memory_buf_t buf;
    fmt::detail::vformat_to(buf, fmt, fmt::make_format_args(args...));
    auto content = std::string{buf.data(), buf.size()};
    return content;
  }

public:
  virtual void Init(std::string flag, LogLevel log_level, LogPosition log_position, LogMode log_mode);
  virtual void InitChannel();
  virtual void AddChannel(ALoggerChannel *channel);

  void SetFlag(STRING flag);
  STRING GetFlag() const;

  void SetLevel(LogLevel level);
  LogLevel GetLevel() const;

  void SetMode(LogMode mode);
  LogMode GetMode() const;

  void SetPosition(LogPosition position);
  LogPosition GetPosition() const;

  void Clean();

protected:
  virtual void WriteMsg(LogLevel level, LogMsgSource prefix, const std::string &content);
};

RD_NAMESPACE_END

#endif//RENDU_BASE_BASE_LOG_A_LOGGER_H_
