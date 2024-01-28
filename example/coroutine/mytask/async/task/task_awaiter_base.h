/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_AWAITER_BASE_H
#define RENDU_TASK_AWAITER_BASE_H

#include "task_executor.h"
#include "task_result.h"

#include <coroutine>

// Base Awaiter
template<typename R>
struct BaseAwaiter {
  BaseAwaiter(){
  }

  virtual ~BaseAwaiter(){
  };

  bool await_ready() {
    return false;
  };

  void await_suspend(std::coroutine_handle<> handle) {
    _handle = handle;
    OnAwaitSuspend();
  }

public:
  void InstallExecutor(AbstractExecutor *executor) {
    _executor = executor;
  }

protected:
  void Dispatch(std::function<void()> &&f) {
    if (!f) {
      return;
    }

    if (_executor) {
      _executor->Execute(std::move(f));
    } else {
      f();
    }
  }

  void ResumeUnsafe() {
    Dispatch([this]() { _handle.resume(); });
  }

protected:
  virtual void OnAwaitSuspend() {}
  virtual void OnAwaitResume() {}

protected:
  void ResumeException(std::exception_ptr &&e) {
    Dispatch([this, e]() {
      _result = Result<R>(static_cast<std::exception_ptr>(e));
      _handle.resume();
    });
  }

protected:
  AbstractExecutor *_executor = nullptr;
  std::coroutine_handle<> _handle = nullptr;

  std::optional<Result<R>> _result{};
};


// Common Awaiter
template<typename R>
struct CommonAwaiter : public BaseAwaiter<R> {

  using ResultType = R;

  R await_resume() {
    this->OnAwaitResume();
    return this->_result->GetOrThrow();
  }

  void resume(R value) {
    Dispatch([this, value]() {
      this->_result = Result<R>(static_cast<R>(value));
      this->_handle.resume();
    });
  }
};


template<>
struct CommonAwaiter<void> : public BaseAwaiter<void> {

  using ResultType = void;

  void await_resume() {
    OnAwaitResume();
    return this->_result->GetOrThrow();
  }

  void resume() {
    Dispatch([this]() {
      this->_result = Result<void>();
      this->_handle.resume();
    });
  }
};

#undef DEFINE_BASE_AWAITER_METHODS

#endif//RENDU_TASK_AWAITER_BASE_H