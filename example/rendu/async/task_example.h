/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_TASK_EXAMPLE_H
#define RENDU_TASK_EXAMPLE_H

#include "log.h"
#include "task/async_task.h"
#include "task/static_thread_pool.h"

using namespace rendu;
using namespace rendu::async;
using namespace rendu::log;

Task<void> DoSomethingVoidAsync() {
  //  LOG_INFO << "start";
  RD_INFO("start");
  //  using namespace std::chrono_literals;
  //  // 模拟一个耗时 3 秒的异步操作
  //  co_await SleepAwait(3s);
  //  std::this_thread::sleep_for(3s);
  // await Task.Delay(3000);
  //  LOG_INFO << "end";
  RD_INFO("end");
  co_return;
}

Task<int> DoSomethingAsync(int v) {
  RD_INFO("start");
  //  LOG_INFO << "start";
  int value = v;
  value++;
  co_await DoSomethingVoidAsync();
  RD_INFO("end");
  //  LOG_INFO << "end";
  co_return value;// 返回一个结果
}

Task<void> Run(int i) {
  int value = i;
  RD_INFO("start");
  //  LOG_INFO << "start";
  //  auto task = DoSomethingAsync(value);
  auto task1 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 Task
  auto task2 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 Task
  RD_INFO("middle");
  //  LOG_INFO << "middle";
  auto result = 0;
  //  auto result = co_await task;
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
//  RD_LOGGER_CLEAN();
//  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
//  RD_LOGGER_INIT("async_example",LogLevel::LL_DEBUG);

  RD_LOGGER_CLEAN();
  RD_LOGGER_SET(new SpdLogger());
  RD_LOGGER_ADD_CHANNEL(new SpdLogConsoleChannel());
  RD_LOGGER_INIT("spd_log",LogLevel::LL_INFO);

  RD_INFO("start")
  Run(0);
  RD_INFO("end")
  return 0;
}

#endif//RENDU_TASK_EXAMPLE_H
