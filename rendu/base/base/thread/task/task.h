/*
* Created by boil on 2023/10/12.
*/

#ifndef RENDU_TASK_H
#define RENDU_TASK_H

#include "task_define.h"
#include <atomic>
#include <cassert>
#include <coroutine>
#include <exception>
#include <iostream>
#include <stdexcept>

RD_NAMESPACE_BEGIN

class broken_promise : public std::logic_error {
public:
  broken_promise()
      : std::logic_error("broken promise") {}
};

template<typename T>
class Task;

namespace detail {

  class task_promise_base {

    friend struct initial_awaitable;

    struct initial_awaitable {
      bool await_ready() const noexcept { return false; }

      template<typename PROMISE>
      void await_suspend(std::coroutine_handle<PROMISE> coroutine) noexcept {
        task_promise_base &promise = coroutine.promise();
        if (promise.m_state.exchange(true, std::memory_order_acq_rel)) {
          promise.m_continuation.resume();
        }
      }

      void await_resume() noexcept {}
    };

    friend struct final_awaitable;

    struct final_awaitable {
      bool await_ready() const noexcept { return false; }

      template<typename PROMISE>
      void await_suspend(std::coroutine_handle<PROMISE> coroutine) noexcept {
        task_promise_base &promise = coroutine.promise();
        if (promise.m_state.exchange(true, std::memory_order_acq_rel)) {
          promise.m_continuation.resume();
        }
      }

      void await_resume() noexcept {}
    };

  public:
    task_promise_base() noexcept
        : m_state(false) {}

    auto initial_suspend() noexcept {
      //      return initial_awaitable{};
      return std::suspend_never{};
    }

    auto final_suspend() noexcept {
      return final_awaitable{};
    }

    bool try_set_continuation(std::coroutine_handle<> continuation) {
      m_continuation = continuation;
      return !m_state.exchange(true, std::memory_order_acq_rel);
    }

  private:
    std::coroutine_handle<> m_continuation;
    std::atomic<bool> m_state;
  };

  template<typename T>
  class task_promise final : public task_promise_base {
  public:
    task_promise() noexcept {
    }

    ~task_promise() {
      switch (m_resultType) {
        case result_type::value:
          m_value.~T();
          break;
        case result_type::exception:
          m_exception.~exception_ptr();
          break;
        default:
          break;
      }
    }

    Task<T> get_return_object() noexcept;

    void unhandled_exception() noexcept {
      ::new (static_cast<void *>(std::addressof(m_exception))) std::exception_ptr(
          std::current_exception());
      m_resultType = result_type::exception;
    }

    template<
        typename VALUE,
        typename = std::enable_if_t<std::is_convertible_v<VALUE &&, T>>>
    void return_value(VALUE &&value) noexcept(std::is_nothrow_constructible_v<T, VALUE &&>) {
      ::new (static_cast<void *>(std::addressof(m_value))) T(std::forward<VALUE>(value));
      m_resultType = result_type::value;
    }

    T &result() & {
      if (m_resultType == result_type::exception) {
        std::rethrow_exception(m_exception);
      }

      assert(m_resultType == result_type::value);

      return m_value;
    }

    using rvalue_type = std::conditional_t<
        std::is_arithmetic_v<T> || std::is_pointer_v<T>,
        T,
        T &&>;

    rvalue_type result() && {
      if (m_resultType == result_type::exception) {
        std::rethrow_exception(m_exception);
      }

      assert(m_resultType == result_type::value);

      return std::move(m_value);
    }

  private:
    enum class result_type {
      empty,
      value,
      exception
    };

    result_type m_resultType = result_type::empty;

    union {
      T m_value;
      std::exception_ptr m_exception;
    };
  };

  template<>
  class task_promise<void> : public task_promise_base {
  public:
    task_promise() noexcept {};

    ~task_promise() {
    }

    Task<void> get_return_object() noexcept;

    void return_void() noexcept {
    }

    void unhandled_exception() noexcept {
      m_exception = std::current_exception();
    }

    void result() {
      if (m_exception) {
        std::rethrow_exception(m_exception);
      }
    }

  private:
    std::exception_ptr m_exception;
  };

  template<typename T>
  class task_promise<T &> : public task_promise_base {
  public:
    task_promise() noexcept {};

    ~task_promise() {
    }

  public:
    Task<T &> get_return_object() noexcept;

    void unhandled_exception() noexcept {
      m_exception = std::current_exception();
    }

    void return_value(T &value) noexcept {
      m_value = std::addressof(value);
    }

    T &result() {
      if (m_exception) {
        std::rethrow_exception(m_exception);
      }

      return *m_value;
    }

  private:
    T *m_value = nullptr;
    std::exception_ptr m_exception;
  };

}// namespace detail

template<typename T = void>
class Task {
public:
  using promise_type = detail::task_promise<T>;
  using coroutine_handle = std::coroutine_handle<promise_type>;

private:
  struct awaitable_base {
    coroutine_handle m_coroutine;

    awaitable_base(std::coroutine_handle<promise_type> coroutine) noexcept
        : m_coroutine(coroutine) {}

    bool await_ready() const noexcept {
      return !m_coroutine || m_coroutine.done();
    }

    bool await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept {
      m_coroutine.resume();
      return m_coroutine.promise().try_set_continuation(awaitingCoroutine);
    }
  };

public:
  Task() noexcept
      : m_coroutine(nullptr) {
  }

  explicit Task(std::coroutine_handle<promise_type> coroutine)
      : m_coroutine(coroutine) {
  }

  Task(Task &&t) noexcept
      : m_coroutine(t.m_coroutine) {
    t.m_coroutine = nullptr;
  }

  /// Disable copy construction/assignment.
  Task(const Task &) = delete;
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

  /// \brief
  /// Query if the Task result is complete.
  ///
  /// Awaiting a Task that is ready is guaranteed not to block/suspend.
  bool is_ready() const noexcept {
    return !m_coroutine || m_coroutine.done();
  }

  auto operator co_await() const & noexcept {
    struct awaitable : awaitable_base {
      using awaitable_base::awaitable_base;

      decltype(auto) await_resume() {
        if (!this->m_coroutine) {
          throw broken_promise{};
        }

        return this->m_coroutine.promise().result();
      }
    };

    return awaitable{m_coroutine};
  }

  auto operator co_await() const && noexcept {
    struct awaitable : awaitable_base {
      using awaitable_base::awaitable_base;

      decltype(auto) await_resume() {
        if (!this->m_coroutine) {
          throw broken_promise{};
        }

        return std::move(this->m_coroutine.promise()).result();
      }
    };
    return awaitable{m_coroutine};
  }

private:
  std::coroutine_handle<promise_type> m_coroutine;
};

namespace detail {
  template<typename T>
  Task<T> task_promise<T>::get_return_object() noexcept {
    return Task{std::coroutine_handle<task_promise>::from_promise(*this)};
  }

  inline Task<void> task_promise<void>::get_return_object() noexcept {
    return Task{std::coroutine_handle<task_promise>::from_promise(*this)};
  }

  template<typename T>
  Task<T &> task_promise<T &>::get_return_object() noexcept {
    return Task{std::coroutine_handle<task_promise>::from_promise(*this)};
  }
}// namespace detail

RD_NAMESPACE_END

#endif//RENDU_TASK_H
