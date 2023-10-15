/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_I_LOG_WRITER_H
#define RENDU_I_LOG_WRITER_H

#include "log_context.h"
#include "utils/noncopyable.h"

RD_NAMESPACE_BEGIN
/**
 * 写日志器
 */
class ILogWriter : public noncopyable {
public:
  ILogWriter() = default;
  virtual ~ILogWriter() = default;

  virtual void Write(const LogContext::Ptr &ctx, Logger &logger) = 0;
};

RD_NAMESPACE_END

#endif //RENDU_I_LOG_WRITER_H
