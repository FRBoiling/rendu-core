/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_TIME_TICKER_H
#define RENDU_TIME_TICKER_H


#include <cassert>
#include "logger/logger.h"
#include "logger/log_context_capture.h"
#include "timer_helper.h"

RD_NAMESPACE_BEGIN

class Ticker {
public:
  /**
   * 此对象可以用于代码执行时间统计，以可以用于一般计时
   * @param min_ms 开启码执行时间统计时，如果代码执行耗时超过该参数，则打印警告日志
   * @param ctx 日志上下文捕获，用于捕获当前日志代码所在位置
   * @param print_log 是否打印代码执行时间
   */
  Ticker(uint64_t min_ms = 0,
         LogContextCapture ctx = LogContextCapture(Logger::Instance(), LogLevel::LWarn, __FILE__, "", __LINE__),
         bool print_log = false);

  ~Ticker();

  /**
   * 获取上次resetTime后至今的时间，单位毫秒
   */
  uint64_t ElapsedTime() const;

  /**
   * 获取从创建至今的时间，单位毫秒
   */
  uint64_t CreatedTime() const;

  /**
   * 重置计时器
   */
  void ResetTime();

private:
  uint64_t _min_ms;
  uint64_t _begin;
  uint64_t _created;
  LogContextCapture _ctx;
};

#if !defined(NDEBUG)
#define TimeTicker() Ticker __ticker(5,LOG_WARN,true)
#define TimeTicker1(tm) Ticker __ticker1(tm,LOG_WARN,true)
#define TimeTicker2(tm, log) Ticker __ticker2(tm,log,true)
#else
  #define TimeTicker()
#define TimeTicker1(tm)
#define TimeTicker2(tm,log)
#endif


RD_NAMESPACE_END

#endif //RENDU_TIME_TICKER_H
