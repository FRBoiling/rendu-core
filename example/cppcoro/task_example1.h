/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_TASK_EXAMPLE_H
#define RENDU_TASK_EXAMPLE_H

#include "cppcoro/static_thread_pool.hpp"
#include "cppcoro/sync_wait.hpp"
#include "cppcoro/task.hpp"
#include "log.hpp"

using namespace cppcoro;
using namespace rendu::log;

static cppcoro::static_thread_pool threadPool{2};

template<class _Fp, class... _Args, typename TResult = std::__invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
static auto Run1(_Fp &&__f, _Args &&...__args) -> TResult {
  co_await threadPool.schedule();
  auto result = co_await __f(__args...);
  co_return result;
}

cppcoro::task<int> ResultVoid(int num) {
  RD_INFO("start");

  RD_INFO("end");
  co_return num;
};

cppcoro::task<int> ResultNum(int num) {
  RD_INFO("start");
  int result = num;
  co_await ResultVoid(result);
  RD_INFO("end");
  result++;
  co_return result;
};

cppcoro::task<int> Run1() {
  RD_INFO("start");
  co_await threadPool.schedule();
  int num = 1;
  num = co_await ResultNum(1);
  RD_INFO("end , result num {}",num);
  co_return num;
};

cppcoro::task<int> Run(){
//  co_return cppcoro::sync_wait(Run1());

}

int task_main() {
  RD_LOGGER_CLEAN();
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("async_example", LogLevel::LL_DEBUG);
  Run1();
  while (true) {
    using namespace std::chrono_literals;
    //    LOG_INFO << "loop ..";
    RD_INFO("loop ..");
    std::this_thread::sleep_for(3s);
  }
  return 0;
}

#endif//RENDU_TASK_EXAMPLE_H
