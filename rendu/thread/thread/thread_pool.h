/*
* Created by boil on 2024/1/21.
*/

#ifndef RENDU_THREAD_THREAD_POOL_H
#define RENDU_THREAD_THREAD_POOL_H

#include "condition.hpp"
#include "mutex_lock.hpp"
#include "thread.h"
#include "thread_define.h"

THREAD_NAMESPACE_BEGIN

class ThreadPool: public NonCopyable {
public:
  ThreadPool(size_t num_threads) : m_stop(false), m_worker_load(num_threads, 0) {
    if(num_threads < 1) {
      throw std::invalid_argument("Number of threads should be >= 1");
    }
    m_worker_threads.resize(num_threads);
    m_tasks.resize(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
      m_worker_threads[i] = std::make_shared<Thread>(std::bind(&ThreadPool::WorkerLoop, this, i));
    }
  }

  ~ThreadPool() {
    m_stop = true;

    for (auto& thread : m_worker_threads) {
      thread->Join();
    }
  }

public:
  template<class F, class... Args>
  void Run(F&& f, Args&&... args) {
    std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    size_t least_loaded_worker = 0;
    //找到负载最低的线程
    for(size_t i = 0; i < m_worker_load.size(); ++i) {
      if(m_worker_load[i] < m_worker_load[least_loaded_worker]) {
        least_loaded_worker = i;
      }
    }
    // Give the job to the "least busy" worker
    {
      std::lock_guard<std::mutex> lock(m_queue_mutex);
      m_tasks[least_loaded_worker].push_back(task);
      ++m_worker_load[least_loaded_worker];
    }
    m_condition.notify_all();
  }

private:
  void WorkerLoop(int workerId) {
    while(!m_stop) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_condition.wait(lock, [this, workerId] { return this->m_stop || !this->m_tasks[workerId].empty(); });
        if(this->m_stop && this->m_tasks.empty()) {
          return;
        }
        task = m_tasks[workerId].front();
        m_tasks[workerId].pop_front();
      }
      try {
        task(); //
        --m_worker_load[workerId];    //任务完成负载--
      } catch(const std::exception& e) {
        RD_CRITICAL("Task threw an exception: {}",e.what() );
        if(!m_stop) {
          // Re-throw exceptions when not stopping
          throw;
        }
      } catch(...) {
        RD_CRITICAL("Task threw an unknown exception: {}");
        if(!m_stop) {
          // Re-throw exceptions when not stopping
          throw;
        }
      }
    }
  }

  std::vector<std::shared_ptr<Thread>> m_worker_threads;
  std::vector<size_t> m_worker_load;
  std::vector<std::deque<std::function<void()>>> m_tasks;
  std::mutex m_queue_mutex;
  std::condition_variable m_condition;
  bool m_stop;
};

extern ThreadPool global_thread_pool;

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_POOL_H
