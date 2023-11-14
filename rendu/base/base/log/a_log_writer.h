/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_I_LOG_WRITER_H
#define RENDU_BASE_I_LOG_WRITER_H

#include "log_define.h"
#include "a_logger.h"

RD_NAMESPACE_BEGIN

/**
 * 写日志器
 */
class ALogWriter : public NonCopyable {
public:
  using Ptr = std::shared_ptr<ALogWriter>;
public:
  ALogWriter() = default;
  virtual ~ALogWriter() = default;

  virtual void Write(const LogContext::Ptr &ctx, ALogger* logger){ };
};

RD_NAMESPACE_END

#endif //RENDU_BASE_I_LOG_WRITER_H
