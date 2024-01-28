/*
* Created by boil on 2024/1/5.
*/

#ifndef RENDU_THREAD_POOL_H
#define RENDU_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <queue>

class ThreadPool {
public:
  ThreadPool(size_t threads):stop(false){
    for(size_t i = 0; i<threads; ++i)
      workers.emplace_back(
          [this]{
            for(;;){
              std::function<void()> task;
              {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock,[this]{ return this->stop || !this->tasks.empty(); });
                if(this->stop && this->tasks.empty())
                  return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
              }
              task();
            }
          });
  }

  template<class F, class... Args>
  auto enqueue(F&& f, Args&&... args){
    std::function<void()> task = [f, args...](){ f(args...); };
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      if(stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");
      tasks.emplace(task);
    }
    condition.notify_one();
  }

  ~ThreadPool(){
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
      worker.join();
  }

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};

extern ThreadPool pool;  // a thread pool with 4 threads



#endif//RENDU_THREAD_POOL_H
