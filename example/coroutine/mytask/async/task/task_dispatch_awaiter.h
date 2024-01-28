/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_DISPATCH_AWAITER_H
#define RENDU_TASK_DISPATCH_AWAITER_H


#include "task_executor.h"
#include <coroutine>

struct DispatchAwaiter {

  explicit DispatchAwaiter(AbstractExecutor *executor) noexcept
      : _executor(executor) {}

  bool await_ready() const { return false; }

  void await_suspend(std::coroutine_handle<> handle) const {
    _executor->Execute([handle]() {
      handle.resume();
    });
  }

  void await_resume() {}

private:
  AbstractExecutor *_executor;
};


#endif//RENDU_TASK_DISPATCH_AWAITER_H
