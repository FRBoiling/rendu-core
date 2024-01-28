/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_SLEEP_AWAITER_H
#define RENDU_TASK_SLEEP_AWAITER_H


#include "task_executor.h"
#include "task_scheduler.h"
#include "task_awaiter_base.h"

struct SleepAwaiter : CommonAwaiter<void> {

  explicit SleepAwaiter(long long duration) noexcept
      : _duration(duration) {}

  template<typename _Rep, typename _Period>
  explicit SleepAwaiter(std::chrono::duration<_Rep, _Period> &&duration) noexcept
      : _duration(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {}

  void OnAwaitSuspend() override {
    static Scheduler scheduler;
    scheduler.Execute([this] { resume(); }, _duration);
  }

  void OnAwaitResume() override {
  }

private:
  long long _duration;
};


#endif//RENDU_TASK_SLEEP_AWAITER_H
