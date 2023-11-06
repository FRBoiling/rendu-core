/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_THREAD_POOL_H
#define RENDU_THREAD_POOL_H

#include "task/task_executor.h"
#include "task/task_queue.h"
#include "thread_group.h"
#include "logger/logger.h"
#include "thread_helper.h"
#include "logger/logger.h"

RD_NAMESPACE_BEGIN

  class ThreadPool : public TaskExecutor {
  public:
    enum Priority {
      PRIORITY_LOWEST = 0,
      PRIORITY_LOW,
      PRIORITY_NORMAL,
      PRIORITY_HIGH,
      PRIORITY_HIGHEST
    };

    explicit ThreadPool(int num = 1, Priority priority = PRIORITY_HIGHEST, bool auto_run = true, bool set_affinity = true,
               const std::string &pool_name = "thread pool");

    ~ThreadPool() override;

    //把任务打入线程池并异步执行
    Task::Ptr Async(TaskIn task, bool may_sync) override;

    Task::Ptr AsyncFirst(TaskIn task, bool may_sync) override;

    size_t size() {
      return _queue.size();
    }

    static bool setPriority(Priority priority = PRIORITY_NORMAL, std::thread::native_handle_type threadId = 0);

    void start() {
      if (_thread_num <= 0) {
        return;
      }
      size_t total = _thread_num - _thread_group.Size();
      for (size_t i = 0; i < total; ++i) {
        _thread_group.CreateThread([this, i]() { run(i); });
      }
    }

  private:
    void run(size_t index);

    void wait() {
      _thread_group.JoinAll();
    }

    void shutdown() {
      _queue.push_exit(_thread_num);
    }

  private:
    size_t _thread_num;
    Logger::Ptr _logger;
    ThreadGroup _thread_group;
    TaskQueue<Task::Ptr> _queue;
    std::function<void(int)> _on_setup;
  };

RD_NAMESPACE_END

#endif //RENDU_THREAD_POOL_H
