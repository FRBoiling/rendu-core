/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_LOG_EXAMPLE_H
#define RENDU_LOG_EXAMPLE_H

#include "log.h"

using namespace rendu;
using namespace rendu::log;

int log_example() {
  //初始化日志模块
  auto logger = new AsyncLogger();
  logger->Add(std::make_shared<ConsoleChannel>());
  logger->SetWriter(std::make_shared<AsyncLogWriter>());
  logger->SetLevel(LogLevel::LTrace);
  LOG_SET_LOGGER(logger);
  LOG_TRACE <<"LOG_TRACE";
  LOG_DEBUG <<"LOG_DEBUG";
  LOG_WARN <<"LOG_WARN";
  LOG_ERROR <<"LOG_ERROR";
  LOG_CRITICAL <<"LOG_CRITICAL";

  return 0;
}

#endif //RENDU_LOG_EXAMPLE_H
