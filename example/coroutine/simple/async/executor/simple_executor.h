/*
* Created by boil on 2023/12/28.
*/

#ifndef RENDU_SIMPLE_EXECUTOR_H
#define RENDU_SIMPLE_EXECUTOR_H

#include "executor.h"
#include "thread/thread_pool.h"
#include "simple_io_executor.h"

// 0xBFFFFFFF == ~0x40000000
inline constexpr int64_t kContextMask = 0x40000000;

// This is a simple executor. The intention of SimpleExecutor is to make the
// test available and show how user should implement their executors. People who
// want to have fun with async_simple could use SimpleExecutor for convenience,
// too. People who want to use async_simple in production level development
// should implement their own executor strategy and implement an Executor
// derived from async_simple::Executor as an interface.
//
// The actual strategy that SimpleExecutor used is implemented in
// async_simple/util/ThreadPool.h.
class SimpleExecutor : public Executor {
public:
  using Func = Executor::Func;
  using Context = Executor::Context;

public:
  explicit SimpleExecutor(size_t threadNum) : _pool(threadNum) {
    _ioExecutor.init();
  }
  ~SimpleExecutor() { _ioExecutor.destroy(); }

public:
  bool schedule(Func func) override {
    return _pool.scheduleById(std::move(func)) ==
           ThreadPool::ERROR_NONE;
  }
  bool currentThreadInExecutor() const override {
    return _pool.getCurrentId() != -1;
  }
  ExecutorStat stat() const override { return ExecutorStat(); }

  size_t currentContextId() const override { return _pool.getCurrentId(); }

  Context checkout() override {
    // avoid CurrentId equal to NULLCTX
    return reinterpret_cast<Context>(_pool.getCurrentId() | kContextMask);
  }

  bool checkin(Func func, Context ctx, ScheduleOptions opts) override {
    int64_t id = reinterpret_cast<int64_t>(ctx);
    auto prompt =
        _pool.getCurrentId() == (id & (~kContextMask)) && opts.prompt;
    if (prompt) {
      func();
      return true;
    }
    return _pool.scheduleById(std::move(func), id & (~kContextMask)) ==
           ThreadPool::ERROR_NONE;
  }

  IOExecutor* getIOExecutor() override { return &_ioExecutor; }

private:
  ThreadPool _pool;
  SimpleIOExecutor _ioExecutor;
};



#endif//RENDU_SIMPLE_EXECUTOR_H
