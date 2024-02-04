/*
* Created by boil on 2023/12/23.
*/

#ifndef RENDU_LOG_MSG_H
#define RENDU_LOG_MSG_H

#include "base/log/log_enum.h"
#include "base/log/log_msg_source.h"
#include "log_msg_time.h"
#include "string/string_helper.h"

LOG_NAMESPACE_BEGIN

#define DEFAULT_PATTERN "{}[{}][{}][{}][{},{}:{}]$ {}"
class LogMsg {
public:
  LogMsg() = default;
  LogMsg(LogMsgTime log_time, LogMsgSource source, string_view_t logger_name, LogLevel lvl, string_view_t content);
  LogMsg(const LogMsg &other) = default;
  LogMsg &operator=(const LogMsg &other) = default;

private:
  string_view_t logger_name_;
  LogLevel lvl_{LogLevel::LL_OFF};
  LogMsgTime time_;
  UINT64  thread_id_;
  LogMsgSource source_;
  string_view_t content_;

public:
  std::string ToString(std::string fmt = DEFAULT_PATTERN);
};

LOG_NAMESPACE_END

#endif//RENDU_LOG_MSG_H
