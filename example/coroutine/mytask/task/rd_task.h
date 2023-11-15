/*
* Created by boil on 2023/12/17.
*/

#ifndef RENDU_RD_TASK_H
#define RENDU_RD_TASK_H


#include "rd_task_promise.h"
#include <coroutine>

template<typename TResult>
struct RDTask {
public:
  using promise_type = RDTaskPromise<TResult>;

public:
  explicit RDTask(std::coroutine_handle<promise_type> handle) noexcept: handle(handle) {}

  RDTask(RDTask &&task) noexcept: handle(std::exchange(task.handle, {})) {}

  RDTask(RDTask &) = delete;

  RDTask &operator=(RDTask &) = delete;

  ~RDTask() {
    if (handle) handle.destroy();
  }

public:
  TResult GetResult() {
    return handle.promise().GetResult();
  }

  RDTask &Then(std::function<void(TResult)> &&func) {
    handle.promise().on_completed([func](auto result) {
      try {
        func(result.get_or_throw());
      } catch (std::exception &e) {
        // ignore.
      }
    });
    return *this;
  }

  RDTask &Catching(std::function<void(std::exception &)> &&func) {
    handle.promise().on_completed([func](auto result) {
      try {
        result.get_or_throw();
      } catch (std::exception &e) {
        func(e);
      }
    });
    return *this;
  }

  RDTask &Finally(std::function<void()> &&func) {
    handle.promise().on_completed([func,this](auto result) { func(); });
    return *this;
  }

  void SetAsync(bool async = true){
    handle.promise().SetAsync(async);
  }

  bool GetAsync(){
    return handle.promise().GetAsync();
  }

private:
  std::coroutine_handle<promise_type> handle;
};


#endif//RENDU_RD_TASK_H
