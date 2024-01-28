/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_H
#define RENDU_TASK_H

#include "convert.h"
#include "task_define.h"
#include "task_executor.h"
#include "task_promise.h"
#include <coroutine>

template<typename ResultType = void, typename Executor = NoopExecutor>
struct [[nodiscard]] Task {

  using promise_type = TaskPromise<ResultType, Executor>;

  auto AsAwaiter() {
    return TaskAwaiter<ResultType, Executor>(*this);
  }

  [[nodiscard]] ResultType Result() {
    if constexpr (!std::is_same_v<ResultType, void>) {
      return m_handle.promise().GetResult();
    }
  }

  [[nodiscard]] const ResultType Result() const {
    if constexpr (!std::is_same_v<ResultType, void>) {
      return m_handle.promise().GetResult();
    }
  }

  void Start() {
    m_handle.resume();
  }

  template<typename _Rep, typename _Period>
  static Task<void, AsyncExecutor> Delay(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    co_await SleepAwaiter(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    co_return;
  }

  template<class _Fp, class... _Args, typename TResult = std::__invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
    requires(std::is_same_v<TResult, Task<ResultType>>)
  static Task<ResultType, AsyncExecutor> Run(_Fp &&__f, _Args &&...__args) {
    co_return co_await __f(__args...);
  }

  template<class _Fp, class... _Args, typename TResult = std::__invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
    requires(!std::is_same_v<TResult, Task<ResultType>>)
  static Task<ResultType, AsyncExecutor> Run(_Fp &&__f, _Args &&...__args) {
    co_return co_await __f(__args...);
  }

  enum class InvokeCondition { IgnoreResult,
                               DontIgnoreResult };

  // 当ResultType为void时，then方法接受不带参数的函数。
  template<typename Func>
    requires(std::is_same_v<ResultType, void>)
  auto Then(Func &&func) -> Task & {
    m_handle.promise().OnCompleted([func = std::move(func)](auto result) {
      try {
        result.GetOrThrow();
        std::invoke(func);
      } catch (std::exception &e) {
        std::cerr << "[Error]: Exception caught in Task: " << e.what() << std::endl;
      }
    });
    return *this;
  }

  // 当ResultType不为void时，then方法接受带ResultType参数的函数。
  template<typename Func>
    requires(!std::is_same_v<ResultType, void>)
  auto Then(Func &&func) -> Task & {
    InvokeWhenNoException<InvokeCondition::DontIgnoreResult>(std::move(func));
    return *this;
  }

  Task &Finally(std::function<void()> &&func) {
    InvokeWhenNoException<InvokeCondition::IgnoreResult>(std::move(func));
    return *this;
  }

  explicit Task(std::coroutine_handle<promise_type> handle) noexcept : m_handle(handle) {
    RD_TRACE("Task::Task() [{}]", Convert::ToString(this));
  }

  Task(Task &&task) noexcept : m_handle(std::exchange(task.m_handle, {})) {
    RD_TRACE("Task::Task() [{}]", Convert::ToString(this));
  }

  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;

  ~Task() {
    if (!Clean()) {
      RD_WARN("Destroying a task that has not completed.");
    }
    RD_TRACE("Task::~Task() [{}]", Convert::ToString(this));
  }

  bool Clean() {
    if (m_handle){
      if (!m_handle.done()) {
        return false;
      }
      RD_TRACE("{}", Convert::ToString(&m_handle));
      m_handle.destroy();
    }
    return true;
  }

private:
  template<InvokeCondition condition, typename Func>
  void InvokeWhenNoException(Func &&func) {
    m_handle.promise().OnCompleted([func = std::move(func)](auto result) {
      try {
        if constexpr (condition == InvokeCondition::DontIgnoreResult) {
          if constexpr (!std::is_same_v<ResultType, void>) {
            std::invoke(func, result.GetOrThrow());
          } else {
            std::invoke(func);
          }
        } else {
          result.GetOrThrow();
          std::invoke(func);
        }
      } catch (std::exception &e) {
        RD_ERROR("Exception caught in Task: {}", e.what());
        //        std::cerr << "[Error]: Exception caught in Task: " << e.what() << std::endl;
      }
    });
  }

private:
  std::coroutine_handle<promise_type> m_handle;
};


#endif//RENDU_TASK_H
