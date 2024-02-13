
#include "example.h"

int main() {
  RD_LOGGER_CLEAN();
  RD_LOGGER_SET(new Logger());
  RD_LOGGER_INIT("base_example", LogLevel::LL_TRACE);

  example();
  return 0;
}