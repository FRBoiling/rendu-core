/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

#include "log.hpp"
#include "thread.hpp"

using namespace rendu;
using namespace rendu::log;
using namespace rendu::thread;
using namespace std::chrono_literals;

int task_main() {
  RD_INFO("start")


  RD_INFO("end")
  return 0;
}

#endif//RENDU_RDTask_EXAMPLE_H
