/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_ASYNC_LOG_WRITER_H
#define RENDU_ASYNC_LOG_WRITER_H

#include "../i_log_writer.h"
#include "../logger.h"
#include "base.h"

LOG_NAMESPACE_BEGIN

  class AsyncLogWriter : public ILogWriter {
  public:
    AsyncLogWriter();

    ~AsyncLogWriter();

  public:
    void Write(const LogContext::Ptr &ctx, Logger &logger) override;

  public:
    void Run();

    void FlushAll();

  private:
    bool _exit_flag;
    Semaphore _sem;
    std::mutex _mutex;
    std::shared_ptr<std::thread> _thread;
    CList<std::pair<LogContext::Ptr, Logger *> > _pending;
  };

LOG_NAMESPACE_END

#endif //RENDU_ASYNC_LOG_WRITER_H
