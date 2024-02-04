/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_LOG_EXAMPLE_H
#define RENDU_LOG_EXAMPLE_H

#include "net.h"

using namespace rendu;
using namespace rendu::net;

int net_example() {
  RD_LOGGER_INIT("net_example", LogLevel::LL_DEBUG);

  return 0;
}


#endif//RENDU_LOG_EXAMPLE_H
