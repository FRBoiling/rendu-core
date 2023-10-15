/*
* Created by boil on 2023/10/25.
*/

#include "thread_load_counter.h"
#include "timer/timer_helper.h"

RD_NAMESPACE_BEGIN

  ThreadLoadCounter::ThreadLoadCounter(uint64_t max_size, uint64_t max_usec) {
    _last_sleep_time = _last_wake_time = GetCurrentMicrosecond();
    _max_size = max_size;
    _max_usec = max_usec;
  }

  void ThreadLoadCounter::StartSleep() {
    std::lock_guard <std::mutex> lck(_mtx);
    _sleeping = true;
    auto current_time = GetCurrentMicrosecond();
    auto run_time = current_time - _last_wake_time;
    _last_sleep_time = current_time;
    _time_list.emplace_back(run_time, false);
    if (_time_list.size() > _max_size) {
      _time_list.pop_front();
    }
  }

  void ThreadLoadCounter::SleepWakeUp() {
    std::lock_guard <std::mutex> lck(_mtx);
    _sleeping = false;
    auto current_time = GetCurrentMicrosecond();
    auto sleep_time = current_time - _last_sleep_time;
    _last_wake_time = current_time;
    _time_list.emplace_back(sleep_time, true);
    if (_time_list.size() > _max_size) {
      _time_list.pop_front();
    }
  }

  int ThreadLoadCounter::CalculateLoad() {
    std::lock_guard <std::mutex> lck(_mtx);
    uint64_t totalSleepTime = 0;
    uint64_t totalRunTime = 0;
    _time_list.For_Each([&](const TimeRecord &rcd) {
      if (rcd._sleep) {
        totalSleepTime += rcd._time;
      } else {
        totalRunTime += rcd._time;
      }
    });

    if (_sleeping) {
      totalSleepTime += (GetCurrentMicrosecond() - _last_sleep_time);
    } else {
      totalRunTime += (GetCurrentMicrosecond() - _last_wake_time);
    }

    uint64_t totalTime = totalRunTime + totalSleepTime;
    while ((_time_list.size() != 0) && (totalTime > _max_usec || _time_list.size() > _max_size)) {
      TimeRecord &rcd = _time_list.front();
      if (rcd._sleep) {
        totalSleepTime -= rcd._time;
      } else {
        totalRunTime -= rcd._time;
      }
      totalTime -= rcd._time;
      _time_list.pop_front();
    }
    if (totalTime == 0) {
      return 0;
    }
    return (int) (totalRunTime * 100 / totalTime);
  }

RD_NAMESPACE_END