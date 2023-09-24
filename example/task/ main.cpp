#include <iostream>
#include "task/task.h"
#include "sync_wait.h"
#include "static_thread_pool.h"
#include "when_all.h"
#include "when_all_ready.h"
#include <thread>

using namespace rendu;

class TaskRun {
private:
  template<typename Func>
  static Task<int> RunOnThreadPool(static_thread_pool &tp, Func func) {
    // 在线程池中调度一个新的协程，执行func并返回其结果
    co_await tp.schedule();
    auto task = co_await func();
    co_return task;
  }

public:
  template<typename Func>
  static Task<int> Run(Func func) {
    static_thread_pool tp;
    // 在线程池中调度一个新的协程，执行func并返回其结果

    auto t = RunOnThreadPool(tp, func);
    co_return sync_wait(t);
  };
};

// 一个用cppcoro::task实现的异步任务函数
Task<int> async_add(int a, int b) {
  std::cout << __FUNCTION__ << "#1 thread id is " << std::this_thread::get_id() << std::endl;
  // 模拟一些耗时的计算
  std::this_thread::sleep_for(std::chrono::seconds(10));
  co_return a + b;
}

int main() {
  // 使用Task类的Run函数启动一个异步任务
  std::cout << __FUNCTION__ << "#1 thread id is " << std::this_thread::get_id() << std::endl;

  auto task = TaskRun::Run([]() -> Task<int> {
    std::cout << "TaskRun" << "#1 thread id is " << std::this_thread::get_id() << std::endl;
    // 调用异步任务函数并等待结果
    auto result = co_await async_add(1, 2);
    // 打印结果
    std::cout << "The result is " << result << "\n";
    co_return result;
  });
  auto result = sync_wait(task);

  std::cout << __FUNCTION__ << "#2 thread id is " << std::this_thread::get_id() << std::endl;
  while (true) {
    std::cout << __FUNCTION__ << " loop thread id is " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
//  // 等待所有任务完成
//  task_scheduler::get_current_task_scheduler()->run_until_idle();
  return 0;
}