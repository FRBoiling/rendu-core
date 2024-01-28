/*
* Created by boil on 2024/1/17.
*/

#ifndef RENDU_THREAD_POOL_H
#define RENDU_THREAD_POOL_H


#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
public:
  ThreadPool(size_t threads) : stop(false) {
    // start worker threads
    for(size_t i = 0;i<threads;++i)
      workers.emplace_back(
          [this] {
            while(true) {
              std::function<void()> task;

              {   // acquire lock
                std::unique_lock<std::mutex> lock(this->queue_mutex);

                // look for a work item
                this->condition.wait(lock,
                                     [this]{ return this->stop || !this->tasks.empty(); });

                // exit if the pool is stopped
                if(this->stop && this->tasks.empty())
                  return;

                // get the task from the queue
                task = std::move(this->tasks.front());
                this->tasks.pop();
              }

              task();
            }
          }
      );
  }

  // add a new work item to the pool
  template<class F, class... Args>
  auto enqueue(F&& f, Args&&... args) {
    using return_type = typename std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mutex);

      // don't allow enqueueing after stopping the pool
      if(stop)  throw std::runtime_error("enqueue on stopped ThreadPool");

      tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
  }

  // the destructor joins all threads
  ~ThreadPool() {
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
  std::queue< std::function<void()> > tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop;
};

#endif//RENDU_THREAD_POOL_H
