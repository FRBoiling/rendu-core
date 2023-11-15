/*
* Created by boil on 2023/11/2.
*/

#include "thread_data.h"
#include "base.h"
#include "exception.h"

ASYNC_NAMESPACE_BEGIN

 ThreadData::ThreadData(ThreadData::ThreadFunc func, const std::string &name, pid_t *tid,
                               CountDownLatch *latch)
    : func_(std::move(func)),
      name_(name),
      tid_(tid),
      latch_(latch) {}

  void ThreadData::runInThread() {
    *tid_ = CurrentThread::tid();
    tid_ = NULL;
    latch_->countDown();
    latch_ = NULL;

    CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
    SetThreadName(CurrentThread::t_threadName);
    try {
      func_();
      CurrentThread::t_threadName = "finished";
    }
    catch (const CException &ex) {
      CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
      abort();
    }
    catch (const std::exception &ex) {
      CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      abort();
    }
    catch (...) {
      CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
      throw; // rethrow
    }
  }


ASYNC_NAMESPACE_END