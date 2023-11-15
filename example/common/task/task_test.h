#include "static_thread_pool.h"
#include "async_task.h"

using namespace rendu;
using namespace rendu::log;

Task<void> DoSomethingVoidAsync() {
//  LOG_INFO << "start";
  PrintI("start");
  //  using namespace std::chrono_literals;
  //  // 模拟一个耗时 3 秒的异步操作
  //  co_await SleepAwait(3s);
  //  std::this_thread::sleep_for(3s);
  // await Task.Delay(3000);
//  LOG_INFO << "end";
  PrintI("end");
  co_return;
}

Task<int> DoSomethingAsync(int v) {
//  LOG_INFO << "start";
  PrintI("start");
  int value = v;
  value++;
  co_await DoSomethingVoidAsync();
//  LOG_INFO << "end";
  PrintI("end");
  co_return value;// 返回一个结果
}

Task<void> Run(int i) {

  int value = i;
//  LOG_INFO << "start";
  PrintI("start");

  //  auto task = DoSomethingAsync(value);
  auto task1 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 Task
  auto task2 = AsyncTask::Run(DoSomethingAsync, 1);// 调用异步方法，返回一个 Task
//  LOG_INFO << "middle";
  PrintI("middle");
  auto result = 0;
  //  auto result = co_await task;
//  LOG_INFO << "end ,result:" << result;
  PrintI("end ,result: %d",result);
  while (true) {
    using namespace std::chrono_literals;
//    LOG_INFO << "loop ..";
    PrintI("loop ...");
    std::this_thread::sleep_for(3s);
  }
}


int task_main() {
    auto logger = new AsyncLogger();
    logger->Add(std::make_shared<ConsoleChannel>());
    logger->SetWriter(std::make_shared<AsyncLogWriter>());
    logger->SetLevel(LogLevel::LTrace);
    LOG_SET_LOGGER(logger);
  LOG_INFO << "start";
  Run(0);
  LOG_INFO << "end";
  return 0;
}