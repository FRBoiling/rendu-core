/*
* Created by boil on 2024/2/6.
*/

#ifndef RENDU_EXAMPLE_TASK_TASK_EXAMPLE1_H_
#define RENDU_EXAMPLE_TASK_TASK_EXAMPLE1_H_

#include "log.hpp"
#include "task.hpp"

using namespace rendu;
using namespace rendu::log;
using namespace rendu::task;
using namespace std::chrono_literals;

// 某个可能需要长时间运行的函数
int LongRunningFunction(int v) {
  int value = v;
  value += 100;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  RD_INFO("sleep end");
  return value;
}

Task<int> DoSomethingAsync(int v) {
  RD_INFO("start");
  //  co_await Task<void>::Delay(3s);
  RD_INFO("end");
  co_return LongRunningFunction(v);
  ;// 返回一个结果
}

Task<void> DoSomethingVoidAsync(int value) {
  RD_INFO("start");
  auto result = co_await DoSomethingAsync(value);
  RD_INFO("result = {} ", result);
  RD_INFO("end");
  co_return;
}

void Loop() {
  RD_INFO("start");
  //  DoSomethingVoidAsync(1);
  //  DoSomethingVoidAsync(2);
  auto task = Task<void>::Run(DoSomethingVoidAsync, 1);// 调用异步方法，返回一个 Task
                                                       //  Task<void>::Run(DoSomethingVoidAsync, 1);   //TODO: 测试 构造和析构 打印this指针地址
                                                       //  Task<void>::Run(DoSomethingVoidAsync, 2);   //TODO: 测试 构造和析构 打印this指针地址
                                                       //  Task<void>::Run(DoSomethingVoidAsync, 2);   //TODO: 测试 构造和析构
                                                       //    DoSomethingVoidAsync(1);

  RD_INFO("middle");
  int i = 0;
  while (i < 10) {
    using namespace std::chrono_literals;
    RD_INFO("loop ..");
    i++;
    std::this_thread::sleep_for(3s);
  }
}


int task_main() {
  RD_INFO("start")
  Loop();
  RD_INFO("end")
  return 0;
}


#endif//RENDU_EXAMPLE_TASK_TASK_EXAMPLE1_H_
