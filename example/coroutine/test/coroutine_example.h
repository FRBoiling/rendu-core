/*
* Created by boil on 2024/1/9.
*/

#ifndef RENDU_COROUTINE_EXAMPLE_H
#define RENDU_COROUTINE_EXAMPLE_H

#include <iostream>
#include <chrono>
#include <future>
#include <coroutine>

using namespace std::chrono_literals;

// 协程任务类型，用于包装 int 协程
template <typename T>
struct CoroTask {
  struct promise_type {
    T value;
    std::exception_ptr exception;
    auto get_return_object() { return CoroTask {handle_type::from_promise(*this)}; }
    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    void unhandled_exception() { exception = std::current_exception(); }
    void return_value(T v) { value = v; }
  };
  using handle_type = std::coroutine_handle<promise_type>;
  handle_type coro;

  CoroTask(handle_type h) : coro(h){}
  CoroTask(CoroTask&& t) : coro(t.coro) { t.coro = nullptr; }
  ~CoroTask() { if (coro) coro.destroy(); }

  bool resume() {
    if (!coro.done())
      coro.resume();
    return !coro.done();
  }

  T result() {
    if (coro.done())
      return coro.promise().value;
    else
      throw std::runtime_error("Coroutine not complete");
  }
};

// 协程函数，模拟一个耗时操作
CoroTask<int> asyncCoroutine() {
  RD_INFO("started");
  std::this_thread::sleep_for(3s); // simulates work
  RD_INFO("ended");
  co_return 42;
}

int coroutine_main() {
  CoroTask<int> task = asyncCoroutine(); // create coroutine
  RD_INFO("started");

  while(task.resume()); // manually resume coroutine until it finishes
  RD_INFO("ended");
  return 0;
}

#endif//RENDU_COROUTINE_EXAMPLE_H
