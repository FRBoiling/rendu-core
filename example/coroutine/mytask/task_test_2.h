/*
* Created by boil on 2023/11/22.
*/

#ifndef RENDU_TASK_TEST_1_H
#define RENDU_TASK_TEST_1_H

#include "task/task.h"
#include <coroutine>
#include <thread>

RDTask<int> DoSomethingAsync(int v) {
  int value = v;
  Log_Info("start");
  //  using namespace std::chrono_literals;
  //  // 模拟一个耗时 3 秒的异步操作
  //  co_await SleepAwait(3s);
  //  co_await SleepAwait(3s);
  //  std::this_thread::sleep_for(3s);

  Log_Info("end");
  value++;
  co_return value;// 返回一个结果
}

RDTask<int> Run(int i) {
  int value = i;
  Log_Info("start");
//  auto task = DoSomethingAsync(value);// 调用异步方法，返回一个 Task
  auto task = Task::Run(DoSomethingAsync, 1);
  auto result = co_await task;
  Log_Info("DoSomethingAsync value is " + std::to_string(value));
  Log_Info("end,result is " + std::to_string(result));
  co_return result;
}


int task_test2() {
  Run(0);
  while (true) {
    using namespace std::chrono_literals;

    std::this_thread::sleep_for(3s);
  }
  return 0;
}


#endif//RENDU_TASK_TEST_1_H
