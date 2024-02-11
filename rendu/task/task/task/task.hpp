/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_TASK_TASK_HPP
#define RENDU_TASK_TASK_HPP

#include "detail/broken_promise.hpp"
#include "detail/awaitable_transform.hpp"
#include "thread/thread_pool.h"

TASK_NAMESPACE_BEGIN
template<typename T>
class Task;

namespace detail {

  class TaskPromiseBase {

    friend struct FinalAwaitable;

    struct FinalAwaitable {
      auto await_ready() const noexcept { return false; }

      template<typename PROMISE>
      std::coroutine_handle<> await_suspend(std::coroutine_handle<PROMISE> coroutine) noexcept {
        static_assert(
            std::is_base_of<TaskPromiseBase, PROMISE>::value,
            "the final awaitable is only allowed to be called by Task");
        auto previous = coroutine.promise().m_previous;
        if (previous) {
          return previous;
        } else {
          return std::noop_coroutine();
        }
      }
      void await_resume() noexcept {}
    };

  public:
    TaskPromiseBase() noexcept {}

    auto initial_suspend() noexcept {
      return std::suspend_never{};
    }

    auto final_suspend() noexcept {
      return FinalAwaitable{};
    }

    void SetContinuation(std::coroutine_handle<> continuation) noexcept {
      m_previous = continuation;
    }

    template<typename Awaitable>
    auto await_transform(Awaitable &&awaitable) {
      return detail::AwaitTransform(std::forward<Awaitable>(awaitable));
    }

  private:
    std::coroutine_handle<> m_previous;
  };

  template<typename T>
  class TaskPromise final : public TaskPromiseBase {
  public:
    static_assert(alignof(T) <= alignof(::max_align_t),
                  "rendu task doesn't allow Task with over aligned object");

    TaskPromise() noexcept = default;
    ~TaskPromise() = default;

  public:
    Task<T> get_return_object() noexcept;

    void unhandled_exception() noexcept {
      m_value.template emplace<std::exception_ptr>(std::current_exception());
    }

    template<typename VALUE, typename = std::enable_if_t<std::is_convertible_v<VALUE &&, T>>>
    void return_value(VALUE &&value) noexcept(std::is_nothrow_constructible_v<T, VALUE &&>)
      requires std::is_convertible_v<VALUE &&, T>
    {
      m_value.template emplace<T>(std::forward<VALUE>(value));
    }

    void SetException(std::exception_ptr exception) {
      m_value.template emplace<std::exception_ptr>(std::current_exception());
    }

    template<typename VALUE, typename = std::enable_if_t<std::is_convertible_v<VALUE &&, T>>>
    void SetValue(VALUE &&value) noexcept(std::is_nothrow_constructible_v<T, VALUE &&>)
      requires std::is_convertible_v<VALUE &&, T>
    {
      m_value.template emplace<T>(std::forward<VALUE>(value));
    }

    T &GetResult() & {
      if (std::holds_alternative<std::exception_ptr>(m_value))
        std::rethrow_exception(std::get<std::exception_ptr>(m_value));
      assert(std::holds_alternative<T>(m_value));
      return std::get<T>(m_value);
    }

    using RValueType = std::conditional_t<std::is_arithmetic_v<T> || std::is_pointer_v<T>, T, T &&>;
    RValueType &&GetResult() && {
      if (std::holds_alternative<std::exception_ptr>(m_value))
        std::rethrow_exception(std::get<std::exception_ptr>(m_value));
      assert(std::holds_alternative<T>(m_value));
      return std::move(std::get<T>(m_value));
    }

  private:
    std::variant<std::monostate, T, std::exception_ptr> m_value;
  };

  template<>
  class TaskPromise<void> : public TaskPromiseBase {
  public:
    TaskPromise() noexcept = default;
    ~TaskPromise() {}

  public:
    Task<void> get_return_object() noexcept;

    void unhandled_exception() noexcept {
      m_exception = std::current_exception();
    }
    void return_void() noexcept {}

    void GetResult() {
      if (m_exception) {
        std::rethrow_exception(m_exception);
      }
    }

    void SetException(std::exception_ptr exception){
        m_exception = exception;
    }

  private:
    std::exception_ptr m_exception;
  };

  template<typename T>
  class TaskPromise<T &> : public TaskPromiseBase {
  public:
    TaskPromise() noexcept = default;
    ~TaskPromise() = default;

  public:
    Task<T &> get_return_object() noexcept;

    void unhandled_exception() noexcept {
      m_value.template emplace<std::exception_ptr>(std::current_exception());
    }

    void return_value(T &value) noexcept {
      m_value = std::addressof(value);
    }

    T &GetResult() & {
      if (std::holds_alternative<std::exception_ptr>(m_value))
        std::rethrow_exception(std::get<std::exception_ptr>(m_value));
      assert(std::holds_alternative<T>(m_value));
      return std::get<T>(m_value);
    }

  private:
    std::variant<std::monostate, T *, std::exception_ptr> m_value;
  };

  template<typename T>
  struct TaskAwaitableBase {

    using CoroutineHandle = std::coroutine_handle<TaskPromise<T>>;
    CoroutineHandle m_coroutine;

    TaskAwaitableBase(CoroutineHandle handle) noexcept
        : m_coroutine(handle) {}

    bool await_ready() const noexcept {
      return !m_coroutine || m_coroutine.done();
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> previous_coroutine) noexcept {
      m_coroutine.promise().SetContinuation(previous_coroutine);
      return m_coroutine;
    }
  };

  template<typename T, bool reschedule>
  class TaskBase {
  public:
    using promise_type = detail::TaskPromise<T>;
    using CoroutineHandle = std::coroutine_handle<promise_type>;

  public:
    CoroutineHandle m_coroutine;

  public:
    TaskBase() noexcept
        : m_coroutine(nullptr) {

      RD_TRACE("TaskBase::TaskBase() [{}]- coroutine is nullptr ", Convert::ToString(this));
    }

    TaskBase(CoroutineHandle coroutine)
        : m_coroutine(coroutine) {
      RD_TRACE("TaskBase::TaskBase() [{}]- coroutine is not nullptr",Convert::ToString(this))
    }

    ~TaskBase() {
      if (m_coroutine) {
        if (m_coroutine.done()) {
          m_coroutine.destroy();
          m_coroutine = nullptr;
          RD_TRACE("TaskBase::~TaskBase() [{}]- coroutine is done",Convert::ToString(this));
        } else {
          RD_WARN("TaskBase::~TaskBase() [{}]- coroutine is not done",Convert::ToString(this))
        }
      }
    }

    /// Disable copy construction/assignment.
    TaskBase(const TaskBase &) = delete;

    TaskBase(TaskBase &&t) noexcept
        : m_coroutine(t.m_coroutine) {
      RD_TRACE("TaskBase::TaskBase(TaskBase &&) - coroutine is not nullptr")
      t.m_coroutine = nullptr;
    }

    TaskBase &operator=(TaskBase &&other) noexcept {
      if (std::addressof(other) != this) {
        if (m_coroutine) {
          if (m_coroutine.done()) {
            m_coroutine.destroy();
            RD_TRACE("TaskBase::operator=(TaskBase &&) - coroutine is done");
          } else {
            RD_WARN("TaskBase::operator=(TaskBase &&) - coroutine is not done");
          }
        }
        m_coroutine = other.m_coroutine;
        other.m_coroutine = nullptr;
      }
      return *this;
    }

    bool IsReady() const noexcept {
      return !m_coroutine || m_coroutine.done();
    }

    struct Awaitable : detail::TaskAwaitableBase<T> {
      using Base = detail::TaskAwaitableBase<T>;
      Awaitable(CoroutineHandle handle) : Base(handle) {}
      decltype(auto) await_resume() {
        if (!this->m_coroutine) {
          throw BrokenPromise{};
        }
        return this->m_coroutine.promise().GetResult();
      }
    };

    auto AsAwaitable() {
      return Awaitable(std::exchange(m_coroutine, nullptr));
    }

    promise_type GetPromise(){
      return m_coroutine.promise();
    }
  };



}// namespace detail

/// \brief
/// A Task represents an operation that produces a result both lazily and taskhronously.
///
/// When you call a coroutine that returns a Task, the coroutine simply captures
/// any passed parameters and returns exeuction to the caller.
/// Execution of the coroutine body does not start until the coroutine is first co_await'ed.


template<typename T = void>
class [[nodiscard]] Task : public detail::TaskBase<T, false> {
public:
  using Base = detail::TaskBase<T, false>;

public:
  template<class _Fp, class... _Args,
           typename TResult = typename std::invoke_result_t<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>>
  static auto Run(_Fp &&__f, _Args &&...__args) {
    auto obj = std::make_shared<TResult>();
    global_thread_pool.Run([obj, &__f, &__args...]() {
      *obj = __f(__args...);
    });
    return obj;
  }

  template<typename _Rep, typename _Period>
  static auto Delay(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    co_return ;
  }



};

namespace detail {
  template<typename T>
  Task<T> TaskPromise<T>::get_return_object() noexcept {
    return Task<T>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

  inline Task<void> TaskPromise<void>::get_return_object() noexcept {
    return Task<void>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

  template<typename T>
  Task<T &> TaskPromise<T &>::get_return_object() noexcept {
    return Task<T &>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }
}// namespace detail

TASK_NAMESPACE_END

#endif
