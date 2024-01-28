/*
* Created by boil on 2023/12/27.
*/

#ifndef RENDU_TASK_HELPER_H
#define RENDU_TASK_HELPER_H

#include "task.h"




  // Returns an awaitable that would return after dur times.
  //
  // e.g. co_await sleep(100s);
  template<typename Rep, typename Period>
  Task<void> sleep(std::chrono::duration<Rep, Period> dur) {
    auto ex = co_await CurrentExecutor();
    if (!ex) {
      std::this_thread::sleep_for(dur);
      co_return;
    }
    co_return co_await ex->after(
        std::chrono::duration_cast<Executor::Duration>(dur));
  }

  template<typename Rep, typename Period>
  Task<void> sleep(Executor *ex, std::chrono::duration<Rep, Period> dur) {
    co_return co_await ex->after(
        std::chrono::duration_cast<Executor::Duration>(dur));
  }




#endif//RENDU_TASK_HELPER_H
