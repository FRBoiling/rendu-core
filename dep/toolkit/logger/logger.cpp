/*
* Created by boil on 2023/10/26.
*/

#include "logger.h"
#include "a_log_channel.h"
#include "i_log_writer.h"
#include "path/path.h"
#include "utils/instance_imp.h"
#include "log_context.h"
#include "log_context_capture.h"

RD_NAMESPACE_BEGIN

  //可重置默认值
  extern Logger *g_defaultLogger;
  Logger *g_defaultLogger = nullptr;

  INSTANCE_IMP(Logger, PathHelper::exeName())

  Logger &GetLogger() {
    if (!g_defaultLogger) {
      g_defaultLogger = &Logger::Instance();
    }
    return *g_defaultLogger;
  }

  void SetLogger(Logger *logger) {
    g_defaultLogger = logger;
  }

  const string &Logger::GetName() const {
    return _logger_name;
  }

  Logger::Logger(const string &loggerName) {
    _logger_name = loggerName;
    _last_log = std::make_shared<LogContext>();
  }

  Logger::~Logger() {
    _writer.reset();
    {
      LogContextCapture(*this, LogLevel::LInfo, __FILE__, __FUNCTION__, __LINE__);
    }
    _channels.clear();
  }


  void Logger::Add(const std::shared_ptr<ALogChannel> &channel) {
    _channels[channel->Name()] = channel;
  }

  void Logger::Remove(const std::string &name) {
    _channels.erase(name);
  }

  std::shared_ptr<ALogChannel> Logger::Get(const string &name) {
    auto it = _channels.find(name);
    if (it == _channels.end()) {
      return nullptr;
    }
    return it->second;
  }

  void Logger::SetWriter(const std::shared_ptr<ILogWriter> &writer) {
    _writer = writer;
  }

  void Logger::Write(const LogContext::Ptr &ctx) {
    if (_writer) {
      _writer->Write(ctx, *this);
    } else {
      writeChannels(ctx);
    }
  }

  void Logger::SetLevel(LogLevel::Level level) {
    for (auto &chn: _channels) {
      chn.second->SetLevel(level);
    }
  }

  void Logger::writeChannels_l(const LogContext::Ptr &ctx) {
    for (auto &chn: _channels) {
      chn.second->Write(*this, ctx);
    }
    _last_log = ctx;
    _last_log->SetRepeat(0);
  }

//返回毫秒
  static int64_t timevalDiff(struct timeval &a, struct timeval &b) {
    return (1000 * (b.tv_sec - a.tv_sec)) + ((b.tv_usec - a.tv_usec) / 1000);
  }

  void Logger::writeChannels(const LogContext::Ptr &ctx) {
    if (ctx->GetLine() == _last_log->GetLine() && ctx->GetFile() == _last_log->GetFile() &&
        ctx->str() == _last_log->str()) {
      //重复的日志每隔500ms打印一次，过滤频繁的重复日志
      _last_log->SetRepeat(_last_log->GetRepeat() + 1);
      if (timevalDiff(_last_log->GetTimeval(), ctx->GetTimeval()) > 500) {
        ctx->SetRepeat(_last_log->GetRepeat());
        writeChannels_l(ctx);
      }
      return;
    }
    if (_last_log->GetRepeat()) {
      writeChannels_l(_last_log);
    }
    writeChannels_l(ctx);
  }


RD_NAMESPACE_END