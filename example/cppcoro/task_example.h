/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_TASK_EXAMPLE_H
#define RENDU_TASK_EXAMPLE_H

#include "log.h"
#include "cppcoro/task.hpp"
#include "async_task.h"
#include "cppcoro/static_thread_pool.hpp"

using namespace cppcoro;
using namespace rendu::log;

task<void> DoSomethingVoidAsync() {
  //  LOG_INFO << "start";
  RD_INFO("start");
  //  using namespace std::chrono_literals;
  //  // 模拟一个耗时 3 秒的异步操作
  //  co_await SleepAwait(3s);
  //  std::this_thread::sleep_for(3s);
  // await task.Delay(3000);
  //  LOG_INFO << "end";
  RD_INFO("end");
  co_return;
}

task<int> DoSomethingAsync(int v) {
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

task<void> Run(int i) {
  int value = i;
  RD_INFO("start");
  //  LOG_INFO << "start";
  co_await DoSomethingAsync(value);
//  auto task1 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 task
//  auto task2 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 task
  RD_INFO("middle");
  //  LOG_INFO << "middle";
  auto result = 0;
//  result = co_await task1;
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
  //  RD_LOGGER_SET(new SpdLogger());
  //  RD_LOGGER_ADD_CHANNEL(new SpdLogConsoleChannel());
  //  RD_LOGGER_INIT("spd_log",LogLevel::LL_INFO);

  RD_INFO("start")
  Run(0);
  RD_INFO("end")
  return 0;
}

#endif//RENDU_TASK_EXAMPLE_H
