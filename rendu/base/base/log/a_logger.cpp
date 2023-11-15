/*
* Created by boil on 2023/10/26.
*/

#include "a_logger.h"
#include "a_log_channel.h"
#include "a_log_writer.h"
#include "log_context.h"
#include "path/path_helper.h"

RD_NAMESPACE_BEGIN

  const std::string &ALogger::GetName() const {
    return _logger_name;
  }

  ALogger::ALogger(const std::string &loggerName) {
    _logger_name = loggerName;
  }

  ALogger::~ALogger() {
    _writer.reset();
    _channels.clear();
  }


  void ALogger::Add(const std::shared_ptr<ALogChannel> &channel) {
    _channels[channel->Name()] = channel;
  }

  void ALogger::Remove(const std::string &name) {
    _channels.erase(name);
  }

  std::shared_ptr<ALogChannel> ALogger::Get(const std::string &name) {
    auto it = _channels.find(name);
    if (it == _channels.end()) {
      return nullptr;
    }
    return it->second;
  }

  void ALogger::SetWriter(const std::shared_ptr<ALogWriter> &writer) {
    _writer = writer;
  }

  void ALogger::SetLevel(LogLevel::Level level) {
    for (auto &chn: _channels) {
      chn.second->SetLevel(level);
    }
  }


  void ALogger::Write(const LogContext::Ptr& ctx) {
    for (auto &chn: _channels) {
      chn.second->Write(ctx, *this);
    }
  }

  //返回毫秒
  static int64_t timevalDiff(struct timeval &a, struct timeval &b) {
    return (1000 * (b.tv_sec - a.tv_sec)) + ((b.tv_usec - a.tv_usec) / 1000);
  }

RD_NAMESPACE_END