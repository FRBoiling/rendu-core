/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

#include "async_example.h"
#include <string>

int add(int i, int j) {
  return i + j;
}

static float minusBy5(float i) {
  return i - 5.0f;
}


class MyFunction {
public:
  std::string concat(std::string& str) const {
    return info_ + str;
  }

  static int multiply(int i, int j) {
    return i * j;
  }

private:
  std::string info_ = "MyFunction : ";
};
//
//// 子线程要执行的任务
//void task(const std::string& name) {
//  for(int i = 0; i < 5; ++i) {
//    // 模拟一些耗时操作
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    SPDLOG_TRACE("线程{}执行中...",name);
//  }
//}
//
//void async_example(){
//  SPDLOG_TRACE("async_example begin...");
//  // 创建两个子线程
//  std::thread thread1(task, "Thread 1");
//  std::thread thread2(task, "Thread 2");
//
//  // 主线程继续执行其它任务
//  for(int i = 0; i < 5; ++i) {
//    // 主线程也执行一些耗时操作
//    std::this_thread::sleep_for(std::chrono::seconds(1));
//    SPDLOG_TRACE("主线程执行中...");
//  }
//
//  // 等待两个子线程执行结束
//  thread1.join();
//  thread2.join();
//  SPDLOG_TRACE("所有线程执行完毕。");
//
//  SPDLOG_TRACE("async_example end.");
//}



#endif//RENDU_RDTask_EXAMPLE_H
