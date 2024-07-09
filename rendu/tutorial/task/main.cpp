#include "task_example1.h"

int main(){
  RD_LOGGER_CLEAN();
  RD_LOGGER_SET(new Logger());
  RD_LOGGER_INIT("async_example",LogLevel::LL_TRACE);

  task_main();
}