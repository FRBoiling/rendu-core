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
  PrintI("start");
  //  using namespace std::chrono_literals;
  //  // 模拟一个耗时 3 秒的异步操作
  //  co_await SleepAwait(3s);
  //  std::this_thread::sleep_for(3s);
  // await Task.Delay(3000);
  //  LOG_INFO << "end";
  PrintI("end");
  co_return;
}

Task<int> DoSomethingAsync(int v) {
  PrintI("start");
  //  LOG_INFO << "start";
  int value = v;
  value++;
  co_await DoSomethingVoidAsync();
  PrintI("end");
  //  LOG_INFO << "end";
  co_return value;// 返回一个结果
}

Task<void> Run(int i) {
  int value = i;
  PrintI("start");
  //  LOG_INFO << "start";
  //  auto task = DoSomethingAsync(value);
  auto task1 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 Task
  auto task2 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 Task
  PrintI("middle");
  //  LOG_INFO << "middle";
  auto result = 0;
  //  auto result = co_await task;
  PrintI("end ,result: %d", result);
  //  LOG_INFO << "end ,result:" << result;
  while (true) {
    using namespace std::chrono_literals;
    //    LOG_INFO << "loop ..";
    PrintI("loop ..");
    std::this_thread::sleep_for(3s);
  }
}


int task_main() {
  auto logger = new AsyncLogger();
  logger->Add(std::make_shared<ConsoleChannel>());
  logger->SetWriter(std::make_shared<AsyncLogWriter>());
  logger->SetLevel(LogLevel::LTrace);
  LOG_SET_LOGGER(logger);
  LOG_INFO << "start";
  Run(0);
  LOG_INFO << "end";
  return 0;
}

#endif//RENDU_TASK_EXAMPLE_H
