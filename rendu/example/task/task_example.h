/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

#include "log.hpp"
#include "task.hpp"

using namespace rendu;
using namespace rendu::log;
using namespace rendu::task;
using namespace std::chrono_literals;

Task<int> DoSomethingAsync(int v) {
  RD_INFO("start");
  int value = v;
  value += 100;
  //  co_await Task<void>::Delay(3s);
  RD_INFO("end");
  co_return value;// 返回一个结果
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
  //  auto Task = Task<void>::Run(DoSomethingVoidAsync, 1);// 调用异步方法，返回一个 Task
  Task<void>::Run(DoSomethingVoidAsync, 1);   //TODO: 测试 构造和析构 打印this指针地址
//  Task<void>::Run(DoSomethingVoidAsync, 2);   //TODO: 测试 构造和析构 打印this指针地址
//  Task<void>::Run(DoSomethingVoidAsync, 2);   //TODO: 测试 构造和析构
//    DoSomethingVoidAsync(1);
  RD_INFO("middle");
  int i = 0;
  while (i<10) {
    using namespace std::chrono_literals;
    RD_INFO("loop ..");
    i++;
    std::this_thread::sleep_for(10s);
  }
}


int task_main() {
  RD_INFO("start")
  Loop();
  RD_INFO("end")
  return 0;
}

#endif//RENDU_RDTask_EXAMPLE_H
