/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_LOG_ASYNC_LOG_WRITER_H
#define RENDU_LOG_ASYNC_LOG_WRITER_H

#include "log_define.h"
#include "base/log/a_log_writer.h"
#include "base/log/a_logger.h"

LOG_NAMESPACE_BEGIN

  class AsyncLogWriter : public ALogWriter {
  public:
    AsyncLogWriter();

    ~AsyncLogWriter();

  public:
    void Write(const LogContext::Ptr &ctx, ALogger* logger) override;

  public:
    void Run();

    void FlushAll();

  private:
    bool _exit_flag;
    Semaphore _sem;
    std::mutex _mutex;
    std::shared_ptr<std::thread> _thread;
    CList<std::pair<LogContext::Ptr, ALogger *> > _pending;
  };

LOG_NAMESPACE_END

#endif //RENDU_LOG_ASYNC_LOG_WRITER_H
