
#include "time_example.h"

int main() {
  RD_LOGGER_CLEAN();
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("async_example", LogLevel::LL_DEBUG);

  time_example();

  return 0;
}