#include "net_example.h"

int main() {
  RD_LOGGER_CLEAN();
  RD_LOGGER_SET(new Logger);

  net_example();
  return 0;
}