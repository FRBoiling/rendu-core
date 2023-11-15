/*
* Created by boil on 2023/11/24.
*/

#ifndef RENDU_RD_TASK_H
#define RENDU_RD_TASK_H


#include "task.h"
#include <coroutine>

#include <chrono>
#include <coroutine>
#include <exception>
#include <future>
#include <iostream>
#include <thread>
#include <type_traits>



// A program-defined type on which the coroutine_traits specializations below depend
struct as_coroutine {};

// Enable the use of std::future<T> as a coroutine type by using a std::promise<T> as the promise type.
template<typename T, typename... Args>
  requires(!std::is_void_v<T> && !std::is_reference_v<T>)
struct std::coroutine_traits<std::future<T>, as_coroutine, Args...> {
  struct promise_type : std::promise<T> {
    std::future<T> get_return_object() noexcept {
      return this->get_future();
    }

    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }

    void return_value(const T &value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
      this->set_value(value);
    }

    void return_value(T &&value) noexcept(std::is_nothrow_move_constructible_v<T>) {
      this->set_value(std::move(value));
    }

    void unhandled_exception() noexcept {
      this->set_exception(std::current_exception());
    }
  };
};

// Same for std::future<void>.
template<typename... Args>
struct std::coroutine_traits<std::future<void>, as_coroutine, Args...> {
  struct promise_type : std::promise<void> {
    std::future<void> get_return_object() noexcept {
      return this->get_future();
    }

    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }

    void return_void() noexcept {
      this->set_value();
    }

    void unhandled_exception() noexcept {
      this->set_exception(std::current_exception());
    }
  };
};

// Allow co_await'ing std::future<T> and std::future<void> by naively spawning a new thread for each co_await.
template<typename T>
auto operator co_await(std::future<T> future) noexcept
  requires(!std::is_reference_v<T>)
{
  struct awaiter : std::future<T> {
    bool await_ready() const noexcept {
      using namespace std::chrono_literals;
      return this->wait_for(0s) != std::future_status::timeout;
    }

    void await_suspend(std::coroutine_handle<> cont) const {
      std::thread([this, cont] {
        this->wait();
        cont();
      }).detach();
    }

    T await_resume() { return this->get(); }
  };

  return awaiter{std::move(future)};
}

class RDTask {
public:
  template<class _Fp, class... _Args>
  static Task<int> Run(_Fp func, _Args... args) {
    // 在线程池中调度一个新的协程，执行func并返回其结果
    int result = co_await std::async(func,args...);
    co_return result;
  };

  template<typename _Rep, typename _Period>
  static Task<void> Delay(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    co_return;
  }
};


#endif//RENDU_RD_TASK_H
