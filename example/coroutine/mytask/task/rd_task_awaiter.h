/*
* Created by boil on 2023/12/17.
*/

#ifndef RENDU_RD_TASK_AWAITER_H
#define RENDU_RD_TASK_AWAITER_H

#include "rd_task_scheduler.h"
#include "task_define.h"
#include <coroutine>


template<typename TResult>
struct RDTask;

template<typename TResult>
struct RDTaskAwaiter {
  explicit RDTaskAwaiter(RDTask<TResult> &&task) noexcept
      : task(std::move(task)) {
    Log_Debug("RDTaskAwaiter +");
  }

  RDTaskAwaiter(RDTaskAwaiter &&completion) noexcept
      : task(std::exchange(completion.task, {})) {
  }

  RDTaskAwaiter(RDTaskAwaiter &) = delete;

  RDTaskAwaiter &operator=(RDTaskAwaiter &) = delete;

  ~RDTaskAwaiter(){
    Log_Debug("RDTaskAwaiter -");
  }
public:
  constexpr bool await_ready() const noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    bool async = task.GetAsync();
    task.Finally([handle,async]() {
      RDTaskScheduler::Dispatch([handle]() {
        handle.resume();
      },async);
    });
  }

  TResult await_resume() noexcept {
    return task.GetResult();
  }

private:
  RDTask<TResult> task;
};


#endif//RENDU_RD_TASK_AWAITER_H
