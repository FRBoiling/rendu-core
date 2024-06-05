/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

//#include "log.hpp"
//#include "thread.hpp"

//using namespace rendu;
//using namespace rendu::log;
//using namespace rendu::thread;
//using namespace std::chrono_literals;

#include <functional>
#include <iostream>
#include <thread>

#include <iostream>
#include <thread>


#include <iostream>
#include <thread>

#include <iostream>
#include <thread>
#include <chrono>

// 子线程要执行的任务
void task(const std::string& name) {
  for(int i = 0; i < 5; ++i) {
    // 模拟一些耗时操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "线程 " << name << " 执行中...\n";
  }
}

int thread_main() {
  // 创建两个子线程
  std::thread thread1(task, "Thread 1");
  std::thread thread2(task, "Thread 2");

  // 主线程继续执行其它任务
  for(int i = 0; i < 5; ++i) {
    // 主线程也执行一些耗时操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "主线程执行中...\n";
  }

  // 等待两个子线程执行结束
  thread1.join();
  thread2.join();

  std::cout << "所有线程执行完毕。\n";

  return 0;
}

#endif//RENDU_RDTask_EXAMPLE_H
