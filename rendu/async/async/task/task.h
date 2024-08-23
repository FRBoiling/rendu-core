/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_ASYNC_TASK_TASK_H_
#define RENDU_ASYNC_ASYNC_TASK_TASK_H_

#include "thread/thread_object.h"
#include "utils/non_copyable.h"

RD_NAMESPACE_BEGIN

class Task : public ThreadObject {
  struct taskBased {
    explicit taskBased() = default;
    virtual Void call() = 0;
    virtual ~taskBased() = default;
  };

  // 退化以获得实际类型
  template<typename F, typename T = typename std::decay<F>::type>
  struct taskDerided : taskBased {
    T func_;
    explicit taskDerided(F &&func)
        : func_(std::forward<F>(func)) {}
    Void call() override { func_(); }
  };

public:
  template<typename F>
  Task(F &&f, int priority = 0)
      : impl_(new taskDerided<F>(std::forward<F>(f))), priority_(priority) {}

  Void operator()() {
    impl_->call();
  }

  Task() = default;

  Task(Task &&task) noexcept : impl_(std::move(task.impl_)),
                               priority_(task.priority_) {}

  Task &operator=(Task &&task) noexcept {
    impl_ = std::move(task.impl_);
    priority_ = task.priority_;
    return *this;
  }

  Bool operator>(const Task &task) const {
    return priority_ < task.priority_;// 新加入的，放到后面
  }

  Bool operator<(const Task &task) const {
    return priority_ >= task.priority_;
  }

  RD_NON_COPYABLE(Task)

private:
  std::unique_ptr<taskBased> impl_ = nullptr;
  int priority_ = 0;// 任务的优先级信息
};


using TaskRef = Task &;
using TaskPtr = Task *;
using TaskArr = std::vector<Task>;
using TaskArrRef = std::vector<Task> &;

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_TASK_TASK_H_
