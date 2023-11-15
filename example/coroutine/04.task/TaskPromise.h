//
// Created by benny on 2022/3/17.
//

#ifndef CPPCOROUTINES_TASKS_04_TASK_TASKPROMISE_H_
#define CPPCOROUTINES_TASKS_04_TASK_TASKPROMISE_H_

#include <functional>
#include <mutex>
#include <list>
#include <optional>

#include "coroutine_common.h"
#include "Result.h"
#include "TaskAwaiter.h"
#include "io_utils.h"

template<typename ResultType>
class Task;

template<typename ResultType>
struct TaskPromise {
  TaskPromise() noexcept {
    debug("task_promise +");
  }

  ~TaskPromise() noexcept {
    debug("task_promise -");
  }
  std::suspend_never initial_suspend() {
    debug("task_promise initial_suspend");
    return {};
  }

  std::suspend_always final_suspend() noexcept {
    debug("task_promise final_suspend");
    return {};
  }

  Task<ResultType> get_return_object() {
    debug("task_promise get_return_object")
    return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

  template<typename _ResultType>
  TaskAwaiter<_ResultType> await_transform(Task<_ResultType> &&task) {
    debug("task_promise await_transform")
    return TaskAwaiter<_ResultType>(std::move(task));
  }

  void unhandled_exception() {
    std::lock_guard lock(completion_lock);
    result = Result<ResultType>(std::current_exception());
    completion.notify_all();
    _notifyCallbacks();
  }

  void return_value(ResultType value) {
    debug("task_promise await_transform")

    std::lock_guard lock(completion_lock);
    result = Result<ResultType>(std::move(value));
    completion.notify_all();
    _notifyCallbacks();
  }

  ResultType GetResult() {
    // blocking for result or throw on exception
    std::unique_lock lock(completion_lock);
    if (!result.has_value()) {
      completion.wait(lock);
    }
    return result->get_or_throw();
  }

  void on_completed(std::function<void(Result<ResultType>)> &&func) {
    std::unique_lock lock(completion_lock);
    if (result.has_value()) {
      auto value = result.value();
      lock.unlock();
      func(value);
    } else {
      completion_callbacks.push_back(func);
    }
  }

 private:
  std::optional<Result<ResultType>> result;

  std::mutex completion_lock;
  std::condition_variable completion;

  std::list<std::function<void(Result<ResultType>)>> completion_callbacks;

  void _notifyCallbacks() {
    auto value = result.value();
    for (auto &callback : completion_callbacks) {
      callback(value);
    }
    completion_callbacks.clear();
  }

};

#endif //CPPCOROUTINES_TASKS_04_TASK_TASKPROMISE_H_
