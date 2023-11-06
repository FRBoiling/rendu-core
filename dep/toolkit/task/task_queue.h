/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_TASK_QUEUE_H
#define RENDU_TASK_QUEUE_H

#include "thread/semaphore.h"
#include "utils/list_exp.h"

RD_NAMESPACE_BEGIN

//实现了一个基于函数对象的任务列队，该列队是线程安全的，任务列队任务数由信号量控制
  template<typename T>
  class TaskQueue {
  public:
    //打入任务至列队
    template<typename C>
    void push_task(C &&task_func) {
      {
        std::lock_guard<decltype(_mutex)> lock(_mutex);
        _queue.emplace_back(std::forward<C>(task_func));
      }
      _sem.Post();
    }

    template<typename C>
    void push_task_first(C &&task_func) {
      {
        std::lock_guard<decltype(_mutex)> lock(_mutex);
        _queue.emplace_front(std::forward<C>(task_func));
      }
      _sem.Post();
    }

    //清空任务列队
    void push_exit(size_t n) {
      _sem.Post(n);
    }

    //从列队获取一个任务，由执行线程执行
    bool get_task(T &tsk) {
      _sem.Wait();
      std::lock_guard<decltype(_mutex)> lock(_mutex);
      if (_queue.empty()) {
        return false;
      }
      tsk = std::move(_queue.front());
      _queue.pop_front();
      return true;
    }

    size_t size() const {
      std::lock_guard<decltype(_mutex)> lock(_mutex);
      return _queue.size();
    }

  private:
    List<T> _queue;
    mutable std::mutex _mutex;
    Semaphore _sem;
  };

RD_NAMESPACE_END

#endif //RENDU_TASK_QUEUE_H
