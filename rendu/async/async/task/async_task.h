/*
* Created by boil on 2023/12/18.
*/

#ifndef RENDU_ASYNC_TASK_H
#define RENDU_ASYNC_TASK_H

#include "task.h"
#include "static_thread_pool.h"
#include "sync_wait.h"


ASYNC_NAMESPACE_BEGIN

class AsyncTask {
public:
  template<class _Fp, class... _Args, typename TResult = std::__invoke_of<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>::type>
  static auto Run(_Fp &&__f, _Args &&...__args) -> TResult {
    static static_thread_pool tp;
    co_await tp.schedule();
    auto task = __f(__args...);
    co_return sync_wait(task);
  }

  template<typename _Rep, typename _Period>
  static Task<void> Delay(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    co_return;
  }
};

ASYNC_NAMESPACE_END


#endif//RENDU_ASYNC_TASK_H
