/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOG_LOGGER_H
#define RENDU_LOG_LOGGER_H

#include "log/a_logger.h"
#include "log_define.h"

LOG_NAMESPACE_BEGIN

/**
* 日志类
*/
class AsyncLogger : public ALogger {

public:
  AsyncLogger();
  friend class AsyncLogWriter;
  using Ptr = std::shared_ptr<AsyncLogger>;

public:
  /**
     * 写日志
     * @param ctx 日志信息
     */
  void Write(const LogContext::Ptr &ctx) override;

private:
  LogContext::Ptr _last_log;

  /**
     * 写日志到各channel，仅供AsyncLogWriter调用
     * @param ctx 日志信息
     */
  void writeChannels(const LogContext::Ptr &ctx);

  void writeChannels_l(const LogContext::Ptr &ctx);
};

LOG_NAMESPACE_END

#endif//RENDU_LOG_LOGGER_H
