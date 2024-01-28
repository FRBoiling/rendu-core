#include "task_example.h"
#include "async_example.h"
#include "coroutine_example.h"
#include "coro_async_example.h"

int main(){
  RD_LOGGER_CLEAN();
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("async_example", LogLevel::LL_DEBUG);

  //  RD_LOGGER_CLEAN();
  //  RD_LOGGER_SET(new SpdLogger());
  //  RD_LOGGER_ADD_CHANNEL(new SpdLogConsoleChannel());
  //  RD_LOGGER_INIT("spd_log",LogLevel::LL_INFO);

//  task_main();
//  async_main();
//  coroutine_main();
  coro_async_main();



  return 0;
}