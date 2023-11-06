/*
* Created by boil on 2023/10/26.
*/

#include "a_log_channel.h"
#include "thread/thread_helper.h"
#include "time/timer_helper.h"
#include "logger.h"

RD_NAMESPACE_BEGIN

  ALogChannel::ALogChannel(const string &name, LogLevel::Level level) : _name(name), _level(level),
                                                                        _enable_detail(true) {}

  ALogChannel::~ALogChannel() {}

  const string &ALogChannel::Name() const { return _name; }

  void ALogChannel::SetLevel(LogLevel::Level level) { _level = level; }

  std::string ALogChannel::PrintTime(const timeval &tv) {
    auto tm = GetLocalTime(tv.tv_sec);
    char buf[128];
    snprintf(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d.%03d",
             1900 + tm.tm_year,
             1 + tm.tm_mon,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec,
             (int) (tv.tv_usec / 1000));
    return buf;
  }

  void ALogChannel::Format(const Logger &logger, std::ostream &ost, const LogContext::Ptr &ctx) {
    if (!_enable_detail && ctx->str().empty()) {
      // 没有任何信息打印
      return;
    }

    // print log time and level
#ifdef _WIN32
    ost << printTime(ctx->_tv) << " " << (char)LOG_CONST_TABLE[ctx->_level][2] << " ";
#else
    ost << PrintTime(ctx->GetTimeval()) << " " << LogLevel::GetLogLevelSimpleName(ctx->GetLogLevel()) << " ";
#endif
    if (_enable_detail) {
      // tag or process name
      ost << "[" << (!ctx->GetFlag().empty() ? ctx->GetFlag() : logger.GetName()) << "] ";
      // pid and thread_name
      ost << "[" << GetPid() << "-" << ctx->GetThreadName() << "] ";
      // source file location
      ost << ctx->GetFile() << ":" << ctx->GetLine() << " " << ctx->GetFunction() << " | ";
    }
    // log content
    ost << ctx->str();
    if (ctx->GetRepeat() > 1) {
      // log repeated
      ost << "\r\n    Last message repeated " << ctx->GetRepeat() << " times";
    }

    // flush log and new line
    ost << std::endl;
  }


RD_NAMESPACE_END