/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_THREAD_LOAD_COUNTER_H
#define RENDU_THREAD_LOAD_COUNTER_H

#include "define.h"
#include "utils/list_exp.h"
#include <mutex>

RD_NAMESPACE_BEGIN

/**
* cpu负载计算器
*/
  class ThreadLoadCounter {
  public:
    /**
     * 构造函数
     * @param max_size 统计样本数量
     * @param max_usec 统计时间窗口,亦即最近{max_usec}的cpu负载率
     */
    ThreadLoadCounter(uint64_t max_size, uint64_t max_usec);

    ~ThreadLoadCounter() = default;
  public:
    /**
     * 线程进入休眠
     */
    void StartSleep();

    /**
     * 休眠唤醒,结束休眠
     */
    void SleepWakeUp();

    /**
     * 返回当前线程cpu使用率，范围为 0 ~ 100
     * @return 当前线程cpu使用率
     */
    int CalculateLoad();

  private:
    struct TimeRecord {
      TimeRecord(uint64_t tm, bool slp) {
        _time = tm;
        _sleep = slp;
      }

      bool _sleep;
      uint64_t _time;
    };

  private:
    bool _sleeping = true;
    uint64_t _last_sleep_time;
    uint64_t _last_wake_time;
    uint64_t _max_size;
    uint64_t _max_usec;
    std::mutex _mtx;
    List<TimeRecord> _time_list;
  };

RD_NAMESPACE_END

#endif //RENDU_THREAD_LOAD_COUNTER_H
