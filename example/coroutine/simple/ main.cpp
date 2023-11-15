#include "debug_log.h"
#include <coroutine>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>

//https://zh.cppreference.com/w/cpp/language/coroutines
auto switch_to_new_thread() {
  struct awaitable {
    bool await_ready() { return false; }//  %%% 这里返回 false，就会先执行await_suspend，之后交出控制权
    void await_suspend(std::coroutine_handle<> h) {
      std::async([h] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Log_Info("work thread");
        //        std::cout << "work thread ID：" << std::this_thread::get_id() << '\n'; // 这样没问题
        h.resume();
      });// %%% 调用了`handle.resume()`，那么这个协程被恢复
    }

    void await_resume() {}
  };
  return awaitable{};
}

struct task {
  struct promise_type {
    task get_return_object() { return {}; }

    std::suspend_never initial_suspend() { return {}; }

    std::suspend_never final_suspend() noexcept { return {}; }

    void return_void() {}

    void unhandled_exception() {}
  };
};

task resuming_on_new_thread() {
  Log_Info("协程开始");
  co_await switch_to_new_thread();
  // 等待器在此销毁
  Log_Info("协程恢复");
}

int main() {
  Log_Info("start 1");
  resuming_on_new_thread();
  Log_Info("loop start");
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    Log_Info("loop ...");
  }
}