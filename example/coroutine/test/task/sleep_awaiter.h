/*
* Created by boil on 2024/1/5.
*/

#ifndef RENDU_SLEEP_AWAITER_H
#define RENDU_SLEEP_AWAITER_H


#include <chrono>
#include <vector>

#include <condition_variable>
#include <functional>
#include <coroutine>
#include "thread_pool.h"

class SleepAwaiter {
public:
  explicit SleepAwaiter(std::chrono::milliseconds timeout)
      : timeout_(timeout) {}

  bool await_ready() const { return timeout_.count() <= 0; }

  void await_suspend(std::coroutine_handle<> coroutine) {
    pool.enqueue([this, coroutine]() {
      std::this_thread::sleep_for(timeout_);
      coroutine.resume();
    });
  }

  void await_resume() const {}

private:
  std::chrono::milliseconds timeout_;
};

SleepAwaiter SleepAwait(std::chrono::milliseconds timeout) {
  return SleepAwaiter(timeout);
}

#endif//RENDU_SLEEP_AWAITER_H
