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
  ThreadPool(size_t numThreads) : stop(false), workerLoad(numThreads, 0) {
    if(numThreads < 1) {
      throw std::invalid_argument("Number of threads should be >= 1");
    }
    workerThreads.resize(numThreads);
    tasks.resize(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
      workerThreads[i] = std::make_shared<Thread>(std::bind(&ThreadPool::workerLoop, this, i));
    }
  }

  ~ThreadPool() {
    stop = true;

    for (auto& thread : workerThreads) {
      thread->Join();
    }
  }

  template<class F, class... Args>
  void Run(F&& f, Args&&... args) {
    std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    size_t leastLoadedWorker = 0;
    // Find the worker that is doing the least work
    for(size_t i = 0; i < workerLoad.size(); ++i) {
      if(workerLoad[i] < workerLoad[leastLoadedWorker]) {
        leastLoadedWorker = i;
      }
    }
    // Give the job to the "least busy" worker
    {
      std::lock_guard<std::mutex> lock(queueMutex);
      tasks[leastLoadedWorker].push_back(task);
      ++workerLoad[leastLoadedWorker];
    }
    condition.notify_all();
  }

private:
  void workerLoop(int workerId) {
    while(!stop) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this, workerId] { return this->stop || !this->tasks[workerId].empty(); });
        if(this->stop && this->tasks.empty()) {
          return;
        }
        task = tasks[workerId].front();
        tasks[workerId].pop_front();
      }
      try {
        task(); // This could throw
        --workerLoad[workerId];
      } catch(const std::exception& e) {
        std::cerr << "Task threw an exception: " << e.what() << std::endl;
        if(!stop) {
          // Re-throw exceptions when not stopping
          throw;
        }
      } catch(...) {
        std::cerr << "Task threw an unknown exception." << std::endl;
        if(!stop) {
          // Re-throw exceptions when not stopping
          throw;
        }
      }
    }
  }

  std::vector<std::shared_ptr<Thread>> workerThreads;
  std::vector<size_t> workerLoad;
  std::vector<std::deque<std::function<void()>>> tasks;
  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop;
};

extern ThreadPool global_thread_pool;

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_POOL_H
