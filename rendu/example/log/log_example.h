/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_LOG_EXAMPLE_H
#define RENDU_LOG_EXAMPLE_H

#include "log.hpp"

using namespace rendu::log;

int log_example() {
  RD_LOGGER_INIT();
  RD_TRACE("{} {} {}", "RD_TRACE", 1, 11);
  RD_DEBUG("{} {} {}", "RD_DEBUG", 2, 22);
  RD_INFO("{} {} {}", "RD_INFO", 2, 22);
  RD_WARN("{} {} {}", "RD_WARN", 3, 33);
  RD_ERROR("{} {} {}", "RD_ERROR", 3, 33);
  RD_CRITICAL("{} {} {}", "RD_ERROR", 3, 33);
//
  RD_LOGGER_CLEAN();
  RD_LOGGER_ADD_CHANNEL(new ConsoleChannel());
  RD_LOGGER_INIT("color_log",LogLevel::LL_DEBUG);
  RD_TRACE("{} {} {}", "RD_TRACE", 1, 11);
  RD_DEBUG("{} {} {}", "RD_DEBUG", 2, 22);
  RD_INFO("{} {} {}", "RD_INFO", 2, 22);
  RD_WARN("{} {} {}", "RD_WARN", 3, 33);
  RD_ERROR("{} {} {}", "RD_ERROR", 3, 33);
  RD_CRITICAL("{} {} {}", "RD_ERROR", 3, 33);

  RD_LOGGER_CLEAN();
  RD_LOGGER_SET(new SpdLogger());
  RD_LOGGER_ADD_CHANNEL(new SpdLogConsoleChannel());
  RD_LOGGER_INIT("spd_log",LogLevel::LL_INFO);
  RD_TRACE("{} {} {}", "RD_TRACE", 1, 11);
  RD_DEBUG("{} {} {}", "RD_DEBUG", 2, 22);
  RD_INFO("{} {} {}", "RD_INFO", 2, 22);
  RD_WARN("{} {} {}", "RD_WARN", 3, 33);
  RD_ERROR("{} {} {}", "RD_ERROR", 3, 33);
  RD_CRITICAL("{} {} {}", "RD_ERROR", 3, 33);

  return 0;
}

//int log_example() {
////  //初始化日志模块
//  auto logger = new AsyncLogger();
//  logger->Add(std::make_shared<ConsoleChannel>());
//  logger->SetWriter(std::make_shared<AsyncLogWriter>());
//  logger->SetLevel(LogLevel::LTrace);
//  LOG_SET_LOGGER(logger);
//
//  LOG_TRACE << "LOG_TRACE";
//  LOG_DEBUG << "LOG_DEBUG";
//  LOG_INFO << "LOG_INFO";
//  LOG_WARN << "LOG_WARN";
//  LOG_ERROR << "LOG_ERROR";
//  LOG_CRITICAL << "LOG_CRITICAL";
//
//  PrintT("%d + %s = %c", LogLevel::LTrace ,"PrintT", 'T');
//  PrintD("%d + %s = %c", LogLevel::LDebug ,"PrintD", 'D');
//  PrintI("%d + %s = %c", LogLevel::LInfo ,"PrintI", 'I');
//  PrintW("%d + %s = %c", LogLevel::LWarn ,"PrintW", 'W');
//  PrintE("%d + %s = %c", LogLevel::LError ,"PrintE", 'E');
//  PrintC("%d + %s = %c", LogLevel::LCritical ,"PrintC", 'C');
//
//  LogT(1, "+", "2", '=', 3);
//  LogD(1, "+", "2", '=', 3);
//  LogI(1, "+", "2", '=', 3);
//  LogW(1, "+", "2", '=', 3);
//  LogE(1, "+", "2", '=', 3);
//  LogC(1, "+", "2", '=', 3);
//
//  auto spdlog = new SpdlogLogger();
//  LOG_SET_LOGGER(spdlog);
//  spdlog->SetLevel(LogLevel::LTrace);
//
//  RD_TRACE("{}{}{}",1,1,1);
//  RD_DEBUG("{}{}{}",1,1,1);
//  RD_WARN("{}{}{}",1,1,1);
//
//  return 0;
//}

#endif//RENDU_LOG_EXAMPLE_H
