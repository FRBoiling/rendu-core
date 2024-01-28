/*
* Created by boil on 2023/12/18.
*/

#ifndef RENDU_ASYNC_TASK_H
#define RENDU_ASYNC_TASK_H

#include "include/cppcoro/task.hpp"
#include "include/cppcoro/sync_wait.hpp"
#include "include/cppcoro/static_thread_pool.hpp"

using namespace cppcoro;
static static_thread_pool tp;

class AsyncTask {
public:
  template<class _Fp, class... _Args, typename TResult = std::__invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
  static auto Run(_Fp &&__f, _Args &&...__args) -> TResult {
    co_await tp.schedule();
    auto task = __f(__args...);
    co_return sync_wait(task);
  }

  template<typename _Rep, typename _Period>
  static task<void> Delay(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    co_return;
  }
};


#endif//RENDU_ASYNC_TASK_H
