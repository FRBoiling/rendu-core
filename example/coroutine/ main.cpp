#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>

//https://zh.cppreference.com/w/cpp/language/coroutines
auto switch_to_new_thread(std::thread &out) {
  struct awaitable {
    std::thread *p_out;

    bool await_ready() { return false; } //  %%% 这里返回 false，就会先执行await_suspend，之后交出控制权
    void await_suspend(std::coroutine_handle<> h) {
      std::thread &out = *p_out;
      if (out.joinable())
        throw std::runtime_error("jthread 输出参数非空");
      out = std::thread([h] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "work thread ID：" << std::this_thread::get_id() << '\n'; // 这样没问题
        h.resume();
      }); // %%% 调用了`handle.resume()`，那么这个协程被恢复
      // 潜在的未定义行为：访问潜在被销毁的 *this
      // std::cout << "新线程 ID：" << p_out->get_id() << '\n';
      std::cout << "新线程 ID：" << out.get_id() << '\n'; // 这样没问题
    }

    void await_resume() {}
  };
  return awaitable{&out};
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

task resuming_on_new_thread(std::thread &out) {
  std::cout << "协程开始，线程 ID：" << std::this_thread::get_id() << '\n';
  co_await switch_to_new_thread(out);
  // 等待器在此销毁
  std::cout << "协程恢复，线程 ID：" << std::this_thread::get_id() << '\n';
}

int main() {
  std::thread out;
  resuming_on_new_thread(out);
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

}