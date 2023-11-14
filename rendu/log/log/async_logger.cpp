/*
* Created by boil on 2023/10/26.
*/


#include "async_logger.h"

#include "path/path_helper.h"

LOG_NAMESPACE_BEGIN

AsyncLogger::AsyncLogger() : ALogger("AsyncLogger") {
  _last_log = std::make_shared<LogContext>();
}

void AsyncLogger::Write(const LogContext::Ptr &ctx) {
  writeChannels(ctx);
}

void AsyncLogger::writeChannels_l(const LogContext::Ptr &ctx) {
  for (auto &chn: _channels) {
    chn.second->Write(ctx, *this);
  }
  _last_log = ctx;
  _last_log->SetRepeat(0);
}

//返回毫秒
static int64_t timevalDiff(struct timeval &a, struct timeval &b) {
  return (1000 * (b.tv_sec - a.tv_sec)) + ((b.tv_usec - a.tv_usec) / 1000);
}

void AsyncLogger::writeChannels(const LogContext::Ptr &ctx) {
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


LOG_NAMESPACE_END