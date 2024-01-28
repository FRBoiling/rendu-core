/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_AWAITER_H
#define RENDU_TASK_AWAITER_H

#include "task_awaiter_base.h"
#include "task_executor.h"

template<typename ResultType, typename Executor>
struct Task;

template<typename R, typename Executor>
struct TaskAwaiter : public CommonAwaiter<R> {
  explicit TaskAwaiter(Task<R, Executor> &&task) noexcept
      : task(std::make_shared<Task<R, Executor>>(std::move(task))) {}

  TaskAwaiter(TaskAwaiter &&awaiter) noexcept
      : CommonAwaiter<R>(awaiter), task(std::move(awaiter.task)) {}

  TaskAwaiter(TaskAwaiter &awaiter) noexcept
      : CommonAwaiter<R>(awaiter), task(awaiter.task) {}

  TaskAwaiter &operator=(TaskAwaiter &) = delete;

protected:
  void OnAwaitSuspend() override {
    task->Finally([this]() {
      this->ResumeUnsafe();
    });
  }

  void OnAwaitResume() override {
    this->_result = Result(task->Result());
  }

private:
  std::shared_ptr<Task<R, Executor>> task;
};

template<typename Executor>
struct TaskAwaiter<void, Executor> : public CommonAwaiter<void> {
  explicit TaskAwaiter(Task<void, Executor> &&task) noexcept
      : task(std::make_shared<Task<void, Executor>>(std::move(task))) {}

  TaskAwaiter(TaskAwaiter &&awaiter) noexcept
      : CommonAwaiter<void>(awaiter), task(std::move(awaiter.task)) {}

  TaskAwaiter(TaskAwaiter &awaiter) noexcept
      : CommonAwaiter<void>(awaiter), task(awaiter.task) {}

  TaskAwaiter &operator=(TaskAwaiter &) = delete;

protected:
  void OnAwaitSuspend() override {
    task->Finally([this]() {
      this->ResumeUnsafe();
    });
  }

  void OnAwaitResume() override {
  }

private:

  std::shared_ptr<Task<void, Executor>> task;
};

#endif//RENDU_TASK_AWAITER_H
