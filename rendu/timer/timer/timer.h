/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_TIMER_TIMER_TIMER_H_
#define RENDU_TIMER_TIMER_TIMER_H_

#include "timer_define.h"
#include <future>

RD_TIMER_NAMESPACE_BEGIN

class Timer : public Object {
public:
  Timer() = default;
  ~Timer() override {
    stop();
  }
  
  /**
     * 开始执行定时器
     * @param interval 间隔时间，单位ms
     * @param task
     */
  template<typename FunctionType>
  Void start(MSec interval, const FunctionType& task) {
    if (!is_stop_.exchange(false)) {
      return;    // 如果正在执行中，则无法继续执行
    }

    left_interval_ = interval;
    origin_interval_ = interval;

    /**
         * std::launch::async：在调用async就开始创建线程。
         * std::launch::deferred：延迟加载方式创建线程。调用async时不创建线程，直到调用了future的get或者wait时才创建线程。
         */
    future_ = std::async(std::launch::async, [this, task]() {
      while (!is_stop_) {
        RD_UNIQUE_LOCK lk(mutex_);
        auto result = cv_.wait_for(lk, std::chrono::milliseconds(left_interval_));
        if (std::cv_status::timeout == result && !is_stop_) {
          MSec start = RD_GET_CURRENT_MS();
          task();
          MSec span = RD_GET_CURRENT_MS() - start;
          /**
                      * 如果任务执行时间 < 设定的时间，则消除任务耗时影响
                      * 如果任务执行时间 > 设定的时间，则继续sleep设定时长
                      */
          left_interval_ = (origin_interval_ > span)
                               ? (origin_interval_ - span)
                               : (origin_interval_);
        }
      }
    });
  }

  /**
     * 关闭定时器
     */
  Void stop() {
    if (is_stop_.exchange(true)) {
      return;
    }

    cv_.notify_one();
    future_.wait();
  }

private:
  std::atomic<Bool> is_stop_ { true };
  std::mutex mutex_;
  std::condition_variable cv_;
  std::future<Void> future_ {};
  MSec origin_interval_ = 0;                 // 设定的耗时信息
  MSec left_interval_ = 0;                   // 除去task运行的时间，还剩下的时间
};
RD_TIMER_NAMESPACE_END



#endif//RENDU_TIMER_TIMER_TIMER_H_
