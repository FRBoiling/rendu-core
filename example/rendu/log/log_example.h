/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_LOG_EXAMPLE_H
#define RENDU_LOG_EXAMPLE_H

#include "log/log.h"

using namespace rendu::log;

int log_example() {
  //初始化日志模块
  Logger::Instance().Add(std::make_shared<ConsoleChannel>());
  Logger::Instance().SetWriter(std::make_shared<AsyncLogWriter>());
  Logger::Instance().SetLevel(LogLevel::LTrace);

  LOG_TRACE <<"LOG_TRACE";
  LOG_DEBUG <<"LOG_DEBUG";
  LOG_WARN <<"LOG_WARN";
  LOG_ERROR <<"LOG_ERROR";
  LOG_CRITICAL <<"LOG_CRITICAL";

  return 0;
}

#endif //RENDU_LOG_EXAMPLE_H
