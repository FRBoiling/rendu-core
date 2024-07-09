/*
* Created by boil on 2024/6/21.
*/

#include "thread_example.h"
#include "thread.h"
#include "thread_pool.h"
#include <spdlog/spdlog.h>


void thread_example() {
  SPDLOG_TRACE("thread_example begin...");
  //  base_thread_example();
//    custom_thread_example();
  //  Thread thread1(rendu_thread_example);
  rendu_thread_example();
  SPDLOG_TRACE("thread_example end.");
}


// 子线程要执行的任务
void task(std::string name) {
  for (int i = 0; i < 5; ++i) {
    // 模拟一些耗时操作
    SPDLOG_TRACE("线程{}执行中...", name);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    SPDLOG_TRACE("线程{}执行完毕", name);
  }
}

void custom_task(std::string task_name) {
  //  for(int i = 0; i < 5; ++i) {
  // 模拟一些耗时操作
  SPDLOG_TRACE("线程{} 执行任务{} 中...", ThisThread::GetCurrentThreadId(), task_name);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  SPDLOG_TRACE("线程{} 执行任务{} 完毕", ThisThread::GetCurrentThreadId(), task_name);

  //  }
}

void task1() {
  for (int i = 0; i < 5; ++i) {
    // 模拟一些耗时操作
    SPDLOG_TRACE("线程 执行中...");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    SPDLOG_TRACE("线程 执行完毕");
  }
}

void base_thread_example() {
  SPDLOG_TRACE("base_thread_example begin...");


  // 创建两个子线程
  std::thread thread1(task, "Thread 1");
  std::thread thread2(task, "Thread 2");

  // 主线程继续执行其它任务
  for (int i = 0; i < 5; ++i) {
    // 主线程也执行一些耗时操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
    SPDLOG_TRACE("主线程执行中...");
  }

  // 等待两个子线程执行结束
  thread1.join();
  thread2.join();
  SPDLOG_TRACE("所有线程执行完毕。");

  SPDLOG_TRACE("base_thread_example end.");
}




void custom_thread_example() {
  SPDLOG_TRACE("custom_thread_example begin...");
  // 创建两个子线程
  Thread thread1(task1);
  Thread thread2(task1);
  thread1.Start();
  thread2.Start();
  // 主线程继续执行其它任务
  for (int i = 0; i < 5; ++i) {
    // 主线程也执行一些耗时操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
    SPDLOG_TRACE("主线程执行中...");
  }

  // 等待两个子线程执行结束
  thread1.Join();
  thread2.Join();
  SPDLOG_TRACE("所有线程执行完毕。");

  SPDLOG_TRACE("custom_thread_example end.");
}

void rendu_thread_example() {
  SPDLOG_TRACE("rendu_thread_example begin...");
  ThreadPool pool(5);
  // 创建两个任务
  pool.submit(custom_task,"pool1111");
  pool.submit(custom_task,"pool1111");
  // 主线程继续执行其它任务
  for (int i = 0; i < 5; ++i) {
    // 主线程也执行一些耗时操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
    SPDLOG_TRACE("主线程执行中...");
  }

  SPDLOG_TRACE("所有线程执行完毕。");

  SPDLOG_TRACE("rendu_thread_example end.");
}
