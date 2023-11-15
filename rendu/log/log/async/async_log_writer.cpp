/*
* Created by boil on 2023/10/26.
*/

#include "async_log_writer.h"
#include "async_logger.h"
#include "thread/thread_helper.h"

LOG_NAMESPACE_BEGIN

AsyncLogWriter::AsyncLogWriter() : _exit_flag(false) {
  _thread = std::make_shared<std::thread>([this]() { this->Run(); });
}

AsyncLogWriter::~AsyncLogWriter() {
  _exit_flag = true;
  _sem.Post();
  _thread->join();
  FlushAll();
}

void AsyncLogWriter::Write(const LogContext::Ptr &ctx, ALogger *logger) {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _pending.emplace_back(std::make_pair(ctx, logger));
  }
  _sem.Post();
}

void AsyncLogWriter::Run() {
  SetCurrThreadName("Async log");
  while (!_exit_flag) {
    _sem.Wait();
    FlushAll();
  }
}

void AsyncLogWriter::FlushAll() {
  decltype(_pending) tmp;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    tmp.swap(_pending);
  }
  tmp.ForEach([&](std::pair<LogContext::Ptr, ALogger *> &pr) {
    AsyncLogger* logger = static_cast<AsyncLogger*> (pr.second);
    logger->writeChannels(pr.first);
  });
}

LOG_NAMESPACE_END