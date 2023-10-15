/*
* Created by boil on 2023/10/26.
*/

#include "thread_pool.h"

RD_NAMESPACE_BEGIN

  ThreadPool::ThreadPool(int num, ThreadPool::Priority priority, bool auto_run, bool set_affinity,
                         const string &pool_name) {
    _thread_num = num;
    _on_setup = [pool_name, priority, set_affinity](int index) {
      std::string name = pool_name + ' ' + std::to_string(index);
      setPriority(priority);
      ThreadHelper::SetThreadName(name.data());
      if (set_affinity) {
        ThreadHelper::SetThreadAffinity(index % std::thread::hardware_concurrency());
      }
    };
    _logger = Logger::Instance().shared_from_this();
    if (auto_run) {
      start();
    }
  }

  ThreadPool::~ThreadPool() {
    shutdown();
    wait();
  }

std::shared_ptr<TaskCancelable<void()>> ThreadPool::Async(TaskIn task, bool may_sync) {
  if (may_sync && _thread_group.IsThisThreadIn()) {
    task();
    return nullptr;
  }
  auto ret = std::make_shared<Task>(std::move(task));
  _queue.push_task(ret);
  return ret;
}

  Task::Ptr ThreadPool::AsyncFirst(TaskIn task, bool may_sync) {
    if (may_sync && _thread_group.IsThisThreadIn()) {
      task();
      return nullptr;
    }

    auto ret = std::make_shared<Task>(std::move(task));
    _queue.push_task_first(ret);
    return ret;
  }

  bool ThreadPool::setPriority(ThreadPool::Priority priority, std::thread::native_handle_type threadId) {
    // set priority
#if defined(_WIN32)
    static int Priorities[] = { THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST };
          if (priority != PRIORITY_NORMAL && SetThreadPriority(GetCurrentThread(), Priorities[priority]) == 0) {
              return false;
          }
          return true;
#else
    static int Min = sched_get_priority_min(SCHED_OTHER);
    if (Min == -1) {
      return false;
    }
    static int Max = sched_get_priority_max(SCHED_OTHER);
    if (Max == -1) {
      return false;
    }
    static int Priorities[] = {Min, Min + (Max - Min) / 4, Min + (Max - Min) / 2, Min + (Max - Min) * 3 / 4, Max};

    if (threadId == 0) {
      threadId = pthread_self();
    }
    struct sched_param params;
    params.sched_priority = Priorities[priority];
    return pthread_setschedparam(threadId, SCHED_OTHER, &params) == 0;
#endif
  }

  void ThreadPool::run(size_t index) {
    _on_setup(index);
    Task::Ptr task;
    while (true) {
      StartSleep();
      if (!_queue.get_task(task)) {
        //空任务，退出线程
        break;
      }
      SleepWakeUp();
      try {
        (*task)();
        task = nullptr;
      } catch (std::exception &ex) {
        ErrorL << "ThreadPool catch a exception: " << ex.what();
      }
    }
  }


RD_NAMESPACE_END