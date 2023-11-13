/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_I_LOG_WRITER_H
#define RENDU_I_LOG_WRITER_H

#include "../../base/base/utils/non_copyable.h"
#include "log_context.h"

LOG_NAMESPACE_BEGIN
/**
 * 写日志器
 */
class ILogWriter : public NonCopyable {
public:
  ILogWriter() = default;
  virtual ~ILogWriter() = default;

  virtual void Write(const LogContext::Ptr &ctx, Logger &logger) = 0;
};

LOG_NAMESPACE_END

#endif //RENDU_I_LOG_WRITER_H
