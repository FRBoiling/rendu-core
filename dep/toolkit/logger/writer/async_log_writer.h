/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_ASYNC_LOG_WRITER_H
#define RENDU_ASYNC_LOG_WRITER_H

#include "i_log_writer.h"
#include "thread/semaphore.h"
#include "utils/list_exp.h"
#include "logger.h"

RD_NAMESPACE_BEGIN

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
    List<std::pair<LogContext::Ptr, Logger *> > _pending;
  };

RD_NAMESPACE_END

#endif //RENDU_ASYNC_LOG_WRITER_H
