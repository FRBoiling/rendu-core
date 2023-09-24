#include <cppcoro/task.hpp>
#include <iostream>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/static_thread_pool.hpp>
// 一个类似于c# 中Task.Run 的函数，接受一个可调用对象作为参数，并返回一个task对象
#include <cppcoro/when_all.hpp>
// 一个异步函数，使用co_await等待task的结果
cppcoro::task<int> async_func() {
  // 使用lambda表达式作为参数，创建一个task对象

  // 等待task的结果
  int result =52;
  std::cout << "Result is " << result << "\n";
}

int main() {

  cppcoro::static_thread_pool threadPool{4};
  // 调用异步函数，并同步等待结果
  cppcoro::sync_wait(async_func());
}
