/*
* Created by boil on 2023/12/17.
*/

#ifndef RENDU_RD_TASK_PROMISE_H
#define RENDU_RD_TASK_PROMISE_H

#include "task_define.h"

#include "rd_task_awaiter.h"
#include "rd_task_result.h"

#include <functional>
#include <list>
#include <mutex>
#include <optional>

template<typename TResult>
struct RDTaskPromise {
  RDTaskPromise() noexcept {
    Log_Debug("task_promise +");
  }

  ~RDTaskPromise() noexcept {
    Log_Debug("task_promise -");
  }

  std::suspend_never initial_suspend() {
    return {};
  }

  std::suspend_always final_suspend() noexcept {
    return {};
  }

  RDTask<TResult> get_return_object() {
    return RDTask{std::coroutine_handle<RDTaskPromise>::from_promise(*this)};
  }

  template<typename _TResult>
  RDTaskAwaiter<_TResult> await_transform(RDTask<_TResult> &&task) {
    return RDTaskAwaiter<_TResult>(std::move(task));
  }

  template<typename _TResult>
  RDTaskAwaiter<_TResult> await_transform(RDTask<_TResult> &task) {
    return RDTaskAwaiter<_TResult>(std::move(task));
  }

  void unhandled_exception() {
    std::lock_guard lock(completion_lock);
    result = RDTaskResult<TResult>(std::current_exception());
    completion.notify_all();
    _notifyCallbacks();
  }

  void return_value(TResult value) {
    std::lock_guard lock(completion_lock);
    result = RDTaskResult<TResult>(std::move(value));
    completion.notify_all();
    _notifyCallbacks();
  }

  TResult GetResult() {
    // blocking for result or throw on exception
    std::unique_lock lock(completion_lock);
    if (!result.has_value()) {
      completion.wait(lock);
    }
    return result->get_or_throw();
  }

  void on_completed(std::function<void(RDTaskResult<TResult>)> &&func) {
    std::unique_lock lock(completion_lock);
    if (result.has_value()) {
      auto value = result.value();
      lock.unlock();
      func(value);
    } else {
      completion_callbacks.push_back(func);
    }
  }

  void SetAsync(bool async = true){
     m_async = async;
  }
  bool GetAsync(){
    return m_async;
  }

private:
  bool m_async;

  std::optional<RDTaskResult<TResult>> result;

  std::mutex completion_lock;
  std::condition_variable completion;

  std::list<std::function<void(RDTaskResult<TResult>)>> completion_callbacks;

  void _notifyCallbacks() {
    auto value = result.value();
    for (auto &callback: completion_callbacks) {
      callback(value);
    }
    completion_callbacks.clear();
  }
};

#endif//RENDU_RD_TASK_PROMISE_H
