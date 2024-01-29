// Task.h

#pragma once

#include "log.hpp"

template<typename T>
class Task;

template<typename T>
class TaskAwaiter;

namespace detail {

  class TaskBrokenPromise : public std::logic_error {
  public:
    TaskBrokenPromise()
        : std::logic_error("Broken promise: The promise has broken unexpectedly") {}
  };

  class TaskPromiseBase {
    struct FinalAwaiter {
      bool await_ready() const noexcept { return false; }
      template<typename PromiseType>
      auto await_suspend(std::coroutine_handle<PromiseType> h) noexcept {
        static_assert(
            std::is_base_of<TaskPromiseBase, PromiseType>::value,
            "the final awaiter is only allowed to be called by Task");

        return h.promise().m_continuation;
      }
      void await_resume() noexcept {}
    };

  public:
    TaskPromiseBase() noexcept {}
    // Lazily started, coroutine will not execute until first resume() is called
    auto initial_suspend() noexcept { return std::suspend_always{}; }
    auto final_suspend() noexcept { return FinalAwaiter{}; }


  public:
    template<typename _ResultType>
    TaskAwaiter<_ResultType> await_transform(Task<_ResultType> &&task);

    template<typename _ResultType>
    TaskAwaiter<_ResultType> await_transform(Task<_ResultType> &task);

  public:
    std::coroutine_handle<> m_continuation;
    void *m_lazy_local;
  };


  template<typename T>
  class TaskPromise : public TaskPromiseBase {
  public:
    static_assert(alignof(T) <= alignof(::max_align_t),
                  "async_simple doesn't allow Task with over aligned object");

    TaskPromise() noexcept {}
    ~TaskPromise() noexcept {}

    Task<T> get_return_object() noexcept;

    template<typename V>
    void return_value(V &&value) noexcept(
        std::is_nothrow_constructible_v<
            T, V &&>)
      requires std::is_convertible_v<V &&, T>
    {
      _value.template emplace<T>(std::forward<V>(value));
    }
    void unhandled_exception() noexcept {
      _value.template emplace<std::exception_ptr>(std::current_exception());
    }


  public:
    T &result() & {
      if (std::holds_alternative<std::exception_ptr>(_value))
        std::rethrow_exception(std::get<std::exception_ptr>(_value));
      assert(std::holds_alternative<T>(_value));
      return std::get<T>(_value);
    }
    T &&result() && {
      if (std::holds_alternative<std::exception_ptr>(_value))
        std::rethrow_exception(std::get<std::exception_ptr>(_value));
      assert(std::holds_alternative<T>(_value));
      return std::move(std::get<T>(_value));
    }


  private:
    std::variant<std::monostate, T, std::exception_ptr> _value;
  };


  template<>
  class TaskPromise<void> : public TaskPromiseBase {
  public:
    TaskPromise() noexcept {}
    ~TaskPromise() noexcept {}

    Task<void> get_return_object() noexcept;

    void return_void() noexcept {}
    void unhandled_exception() noexcept {
      _exception = std::current_exception();
    }

    void result() {
      if (_exception != nullptr)
        std::rethrow_exception(_exception);
    }

  public:
    std::exception_ptr _exception{nullptr};
  };

  template<typename T>
  struct TaskAwaiterBase {

    using Handle = std::coroutine_handle<TaskPromise<T>>;
    Handle _handle;

    TaskAwaiterBase(Task<T> &&task) noexcept;
    TaskAwaiterBase(Task<T> &task) noexcept;

    TaskAwaiterBase(TaskAwaiterBase &other) = delete;
    TaskAwaiterBase &operator=(TaskAwaiterBase &other) = delete;

    TaskAwaiterBase(TaskAwaiterBase &&other)
        : _handle(std::exchange(other._handle, nullptr)) {}

    TaskAwaiterBase &operator=(TaskAwaiterBase &&other) {
      std::swap(_handle, other._handle);
      return *this;
    }

    TaskAwaiterBase(Handle coro) : _handle(coro) {}
    ~TaskAwaiterBase() {
      if (_handle) {
        _handle.destroy();
        _handle = nullptr;
      }
    }

    bool await_ready() const noexcept { return false; }

    auto awaitResume() {
      if constexpr (std::is_void_v<T>) {
        _handle.promise().result();
        // We need to destroy the handle expclictly since the awaited
        // coroutine after symmetric transfer couldn't release it self any
        // more.
        _handle.destroy();
        _handle = nullptr;
      } else {
        auto r = std::move(_handle.promise()).result();
        _handle.destroy();
        _handle = nullptr;
        return r;
      }
    }

  protected:
    Task<T> _task;
  };


}// namespace detail

template<typename T>
struct TaskAwaiter : public detail::TaskAwaiterBase<T> {

  using Base = detail::TaskAwaiterBase<T>;

  explicit TaskAwaiter(Task<T> &task) noexcept;

  explicit TaskAwaiter(Task<T> &&task) noexcept;

  TaskAwaiter(TaskAwaiter &&completion) noexcept;

  TaskAwaiter(TaskAwaiter &) = delete;

  TaskAwaiter(Base::Handle coro) : Base(coro) {}

  TaskAwaiter &operator=(TaskAwaiter &) = delete;

  constexpr bool await_ready() const noexcept {
    return false;
  }

  template<typename PromiseType>
  auto await_suspend(std::coroutine_handle<PromiseType> continuation) noexcept {
    // current coro started, caller becomes my continuation
    this->_handle.promise().m_continuation = continuation;
    if constexpr (std::is_base_of<detail::TaskPromiseBase, PromiseType>::value) {
      this->_handle.promise().m_lazy_local = continuation.promise().m_lazy_local;
    }
    return awaitSuspendImpl();
  }

  T await_resume() { return Base::awaitResume(); }

private:
  auto awaitSuspendImpl() noexcept {
//    if (Base::_task.m_reschedule) {
//      // executor schedule performed
//      //      auto &pr = this->_handle.promise();
//      //      logicAssert(pr._executor, "RescheduleTask need executor");
//      //      pr._executor->schedule(this->_handle);
//    } else {
      return this->_handle;
//    }
  }
};


template<typename T = void>
class [[nodiscard]] Task {
public:
  using promise_type = detail::TaskPromise<T>;
  using coroutine_handle = std::coroutine_handle<promise_type>;

public:
  Task() noexcept
      : m_coroutine(nullptr) {}

  explicit Task(std::coroutine_handle<promise_type> coroutine)
      : m_coroutine(coroutine) {}

  Task(Task &&task) noexcept : m_coroutine(std::exchange(task.m_coroutine, {})) {}

  //  Task(Task &) = delete;
  /// Disable copy construction/assignment.
  //  Task(const Task &) = delete;
  Task(const Task &t) : m_coroutine(t.m_coroutine) {}

  Task &operator=(const Task &) = delete;

  /// Frees resources used by this Task.
  ~Task() {
    if (m_coroutine) {
      m_coroutine.destroy();
    }
  }

  Task &operator=(Task &&other) noexcept {
    if (std::addressof(other) != this) {
      if (m_coroutine) {
        m_coroutine.destroy();
      }

      m_coroutine = other.m_coroutine;
      other.m_coroutine = nullptr;
    }

    return *this;
  }


public:
  std::coroutine_handle<promise_type> m_coroutine;
  bool m_reschedule;
};

template<typename T>
TaskAwaiter<T>::TaskAwaiter(Task<T> &&task) noexcept
    : Base(std::move(task)) {}

template<typename T>
TaskAwaiter<T>::TaskAwaiter(Task<T> &task) noexcept
    : Base(task) {}


namespace detail {
  template<typename T>
  TaskAwaiterBase<T>::TaskAwaiterBase(Task<T> &&task) noexcept
      : _task(std::move(task)) {}

  template<typename T>
  TaskAwaiterBase<T>::TaskAwaiterBase(Task<T> &task) noexcept
      : _task(task) {}

  template<typename _ResultType>
  TaskAwaiter<_ResultType> TaskPromiseBase::await_transform(Task<_ResultType> &&task) {
    return TaskAwaiter<_ResultType>(std::move(task));
  }

  template<typename _ResultType>
  TaskAwaiter<_ResultType> TaskPromiseBase::await_transform(Task<_ResultType> &task) {
    return TaskAwaiter<_ResultType>(task);
  }

  template<typename T>
  Task<T> TaskPromise<T>::get_return_object() noexcept {
    return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

  Task<void> TaskPromise<void>::get_return_object() noexcept {
    return Task<void>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

}// namespace detail