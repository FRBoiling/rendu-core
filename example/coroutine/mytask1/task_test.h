/*
* Created by boil on 2023/11/22.
*/

#ifndef RENDU_TASK_TEST_1_H
#define RENDU_TASK_TEST_1_H

#include <coroutine>
#include <thread>
#include "rd_task.h"

class TaskTest{
public:
  static Task<int> DoSomethingAsync(int v) {
    Log_Info("start");
    int value = v;
    using namespace std::chrono_literals;
    using namespace std::chrono_literals;
    // 模拟一个耗时 3 秒的异步操作
    co_await RDTask::Delay(3s);
    //  co_await SleepAwait(3s);
    //  std::this_thread::sleep_for(3s);
    value++;
    Log_Info("end");
    co_return value;// 返回一个结果
  }

  static int DoSomething(int v) {
    Log_Info("start");
    int value = v;
    using namespace std::chrono_literals;
    using namespace std::chrono_literals;
    // 模拟一个耗时 3 秒的异步操作
//    co_await RDTask::Delay(3s);
    //  co_await SleepAwait(3s);
    //  std::this_thread::sleep_for(3s);
    value++;
    Log_Info("end");
    return value;// 返回一个结果
  }

  static Task<void> Run() {
    Log_Info("start");
    int value = 0;
    Log_Info("before await: Task.Run");
//    auto result = co_await DoSomethingAsync(value);
    int result = co_await RDTask::Run(DoSomethingAsync,value); //等待一个异步结果
    Log_Info("after await: Task.Run ");
    Log_Info("end,result is " + std::to_string(result));
    co_return;
  }
};

#endif//RENDU_TASK_TEST_1_H
