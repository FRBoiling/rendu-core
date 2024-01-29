#include <iostream>
#include <future>
#include <thread>
#include <functional>

#include "log.hpp"
using namespace rendu::log;

// AsyncTask 类，表示一个异步任务
class AsyncTask {
public:
  // 构造函数，接受一个函数对象作为任务的执行函数
  AsyncTask(std::function<int()> func) : func_(std::move(func)) {}

  // 启动任务的执行
  void start() {
    future_ = std::async(std::launch::async, func_);
  }

  // 获取任务的结果
  int getResult() {
    // 等待未来结果完成
    future_.wait();
    // 返回未来结果的值
    return future_.get();
  }

private:
  // 任务的执行函数
  std::function<int()> func_;
  // 任务的未来结果
  std::future<int> future_;
};

int asyncFunction() {
  RD_INFO("start");
  // 模拟耗时操作
  std::this_thread::sleep_for(std::chrono::seconds(3));
  RD_INFO("end");
  return 42;
}

// 主函数
int async_main() {
  RD_INFO("start");
  AsyncTask task(asyncFunction);
  // 启动任务
  task.start();
  RD_INFO("middle");
  // 等待 1 秒，让任务有足够的时间完成
//  std::this_thread::sleep_for(std::chrono::seconds(1));
  // 获取任务的结果
//  int result = task.getResult();
  RD_INFO("end");
  return 0;
}