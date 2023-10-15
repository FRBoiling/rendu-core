/*
* Created by boil on 2023/10/26.
*/

#include "a_log_channel.h"
#include "thread/thread_helper.h"
#include "timer/timer_helper.h"
#include "logger.h"

RD_NAMESPACE_BEGIN

  ALogChannel::ALogChannel(const string &name, LogLevel level) : _name(name), _level(level) {}

  ALogChannel::~ALogChannel() {}

  const string &ALogChannel::name() const { return _name; }

  void ALogChannel::setLevel(LogLevel level) { _level = level; }

  std::string ALogChannel::printTime(const timeval &tv) {
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

#ifdef _WIN32
#define printf_pid() GetCurrentProcessId()
#else
#define printf_pid() getpid()
#endif

  void ALogChannel::Format(const Logger &logger, std::ostream &ost, const LogContext::Ptr &ctx, bool enable_color,
                           bool enable_detail) {
    if (!enable_detail && ctx->str().empty()) {
      // 没有任何信息打印
      return;
    }

    if (enable_color) {
      // color console start
#ifdef _WIN32
      SetConsoleColor(LOG_CONST_TABLE[ctx->_level][1]);
#else
      ost << LOG_CONST_TABLE[ctx->GetLogLevel()][1];
#endif
    }

    // print log time and level
#ifdef _WIN32
    ost << printTime(ctx->_tv) << " " << (char)LOG_CONST_TABLE[ctx->_level][2] << " ";
#else
    ost << printTime(ctx->GetTimeval()) << " " << LOG_CONST_TABLE[ctx->GetLogLevel()][2] << " ";
#endif

    if (enable_detail) {
      // tag or process name
      ost << "[" << (!ctx->GetFlag().empty() ? ctx->GetFlag() : logger.GetName()) << "] ";
      // pid and thread_name
      ost << "[" << GetCurrentProcessId() << "-" << ctx->GetThreadName() << "] ";
      // source file location
      ost << ctx->GetFile() << ":" << ctx->GetLine() << " " << ctx->GetFunction() << " | ";
    }

    // log content
    ost << ctx->str();

    if (enable_color) {
      // color console end
#ifdef _WIN32
      SetConsoleColor(CLEAR_COLOR);
#else
      ost << CLEAR_COLOR;
#endif
    }

    if (ctx->GetRepeat() > 1) {
      // log repeated
      ost << "\r\n    Last message repeated " << ctx->GetRepeat() << " times";
    }

    // flush log and new line
    ost << std::endl;
  }

RD_NAMESPACE_END