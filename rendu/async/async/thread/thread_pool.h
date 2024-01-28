/*
* Created by boil on 2024/1/21.
*/

#ifndef RENDU_ASYNC_THREAD_POOL_H
#define RENDU_ASYNC_THREAD_POOL_H

#include "async_define.h"
#include <atomic>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

ASYNC_NAMESPACE_BEGIN

class ThreadPool {
private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queue_mtx;
  std::condition_variable condition;
  std::atomic_bool stop;

public:
  explicit ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i)
      workers.emplace_back(
          [this] {
            for (;;) {
              std::function<void()> task;
              {
                std::unique_lock<std::mutex> lock(this->queue_mtx);
                this->condition.wait(lock, [this] { return this->stop.load() || !this->tasks.empty(); });
                if (this->stop.load() && this->tasks.empty())
                  return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
              }
              task();
            }
          });
  }

  template<class _Fp, class... _Args,
           typename TResult = typename std::invoke_result_t<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>>
  auto Enqueue(_Fp &&f, _Args &&...args) -> std::future<TResult> {
    auto task = std::make_shared<std::packaged_task<TResult()>>(
        std::bind(std::forward<_Fp>(f), std::forward<_Args>(args)...));
    std::future<TResult> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mtx);
      if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");
      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
  }

  ~ThreadPool() {
    stop.store(true);
    condition.notify_all();
    for (std::thread &worker: workers)
      worker.join();
  }
};

extern ThreadPool global_thread_pool;

ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_THREAD_POOL_H
