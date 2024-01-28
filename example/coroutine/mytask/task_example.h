/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

#include "log.h"
#include "task/task.h"

using namespace rendu;
using namespace rendu::log;
using namespace std::chrono_literals;


Task<int> DoSomethingAsync(int v) {
  RD_DEBUG("start");
  int value = v;
  value++;
  co_await Task<void>::Delay(3s);
  RD_DEBUG("end");
  co_return value;// 返回一个结果
}

Task<void> DoSomethingVoidAsync(int value) {
  RD_DEBUG("start");
  auto result = co_await DoSomethingAsync(value);
  RD_DEBUG("end ,result = {} ",result);
  co_return;
}

void Run() {
  RD_INFO("start");
  Task<void>::Run(DoSomethingVoidAsync, 1);// 调用异步方法，返回一个 Task
  RD_INFO("middle");
  while (true) {
    using namespace std::chrono_literals;
    RD_INFO("loop ..");
    std::this_thread::sleep_for(10s);
  }
}

int task_main() {
//  RD_LOGGER_CLEAN();
//  RD_LOGGER_SET(new SpdLogger());
//  RD_LOGGER_ADD_CHANNEL(new SpdLogConsoleChannel());
//  RD_LOGGER_INIT("spd_log",LogLevel::LL_INFO);

  RD_INFO("start")
  Run();
  RD_INFO("end")
  return 0;
}

#endif//RENDU_RDTask_EXAMPLE_H
