/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

#include "log.hpp"
#include "task/task.h"

using namespace rendu;
using namespace rendu::log;

Task<void> DoSomethingVoidAsync() {
  //  LOG_INFO << "start";
  RD_INFO("start");
  using namespace std::chrono_literals;
  //  // 模拟一个耗时 3 秒的异步操作
  //  co_await SleepAwait(3s);
  //  std::this_thread::sleep_for(3s);
  //  AsyncExecutor executor;
  //  co_await async::sleep(&executor,3s);
  //  LOG_INFO << "end";
  RD_INFO("end");
  co_return;
}

Task<int> DoSomethingAsync(int v) {
  RD_INFO("start");
  //  LOG_INFO << "start";
  int value = v;
  value++;
  auto task = DoSomethingVoidAsync();
  co_await task;
  RD_INFO("end");
  //  LOG_INFO << "end";
  co_return value;// 返回一个结果
}

Task<void> Run(int i) {
  int value = i;
  RD_INFO("start");
  //  LOG_INFO << "start";
  auto task = DoSomethingAsync(value);
  //  auto task1 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 task
  //  auto task2 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 task
  RD_INFO("middle");
  //  LOG_INFO << "middle";
  auto result = 0;
  result = co_await task;
  //  result = co_await task1;
  //  result = co_await task2;
  RD_INFO("end ,result: {}", result);
  //  LOG_INFO << "end ,result:" << result;
  while (true) {
    using namespace std::chrono_literals;
    //    LOG_INFO << "loop ..";
    RD_INFO("loop ..");
    std::this_thread::sleep_for(3s);
  }
}


int task_main() {


  RD_INFO("start")
  auto task = Run(0);
  RD_INFO("end")
  return 0;
}



#endif//RENDU_RDTask_EXAMPLE_H
