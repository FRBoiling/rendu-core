/*
* Created by boil on 2023/12/27.
*/

#ifndef RENDU_DETACHED_COROUTINE_H
#define RENDU_DETACHED_COROUTINE_H

#include <atomic>
#include <coroutine>
#include <exception>
#include <mutex>
#include <stdio.h>

namespace detail {
  struct DetachedCoroutine {
    struct promise_type {
      std::suspend_never initial_suspend() noexcept { return {}; }
      std::suspend_never final_suspend() noexcept { return {}; }
      void return_void() noexcept {}
      void unhandled_exception() {
        try {
          std::rethrow_exception(std::current_exception());
        } catch (const std::exception &e) {
          fprintf(stderr, "find exception %s\n", e.what());
          fflush(stderr);
          std::rethrow_exception(std::current_exception());
        }
      }
      DetachedCoroutine get_return_object() noexcept {
        return DetachedCoroutine();
      }

      // Hint to gdb script for that there is no continuation for
      // DetachedCoroutine.
      std::coroutine_handle<> _continuation = nullptr;
      void *_lazy_local = nullptr;
    };
  };

}// namespace detail

// This allows we to co_await a non-awaitable. It would make
// the co_await expression to return directly.
template<typename T>
struct ReadyAwaiter {
  ReadyAwaiter(T value) : _value(std::move(value)) {}

  bool await_ready() const noexcept { return true; }
  void await_suspend(std::coroutine_handle<>) const noexcept {}
  T await_resume() noexcept { return std::move(_value); }

  T _value;
};

#endif//RENDU_DETACHED_COROUTINE_H
