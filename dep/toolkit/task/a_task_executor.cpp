/*
* Created by boil on 2023/10/25.
*/

#include "a_task_executor.h"
#include "thread/semaphore.h"
#include "utils/once_token.h"

RD_NAMESPACE_BEGIN
  Task::Ptr ATaskExecutor::AsyncFirst(TaskIn task, bool may_sync) {
    return Async(std::move(task), may_sync);
  }

  void ATaskExecutor::Sync(const TaskIn &task) {
    Semaphore sem;
    auto ret = Async([&]() {
      OnceToken token(nullptr, [&]() {
        //通过RAII原理防止抛异常导致不执行这句代码
        sem.Post();
      });
      task();
    });
    if (ret && *ret) {
      sem.Wait();
    }
  }

  void ATaskExecutor::SyncFirst(const TaskIn &task) {
    Semaphore sem;
    auto ret = AsyncFirst([&]() {
      OnceToken token(nullptr, [&]() {
        //通过RAII原理防止抛异常导致不执行这句代码
        sem.Post();
      });
      task();
    });
    if (ret && *ret) {
      sem.Wait();
    }
  }

RD_NAMESPACE_END