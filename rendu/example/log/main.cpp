
#include "log.h"
#include "log_example.h"
#include <thread>

int main() {
  RD_LOGGER_CLEAN();
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("async_example", LogLevel::LL_DEBUG);

  log_example();
  return 0;
}