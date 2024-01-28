/*
* Created by boil on 2024/1/10.
*/

#ifndef RENDU_TASK_EXECUTOR_H
#define RENDU_TASK_EXECUTOR_H

#include "thread_pool.h"
#include <functional>
#include <future>
#include <mutex>
#include <queue>


class AbstractExecutor {
public:
  using Ptr = std::shared_ptr<AbstractExecutor>;

public:
  virtual void Execute(std::function<void()> &&func) = 0;
};

using Executor = AbstractExecutor;

class NoopExecutor : public AbstractExecutor {
public:
  void Execute(std::function<void()> &&func) override {
    func();
  }
};

class NewThreadExecutor : public AbstractExecutor {
public:
public:
  void Execute(std::function<void()> &&func) override {

    std::thread(func).detach();
  }
};

class AsyncExecutor : public AbstractExecutor {
public:
  void Execute(std::function<void()> &&func) override {
    static ThreadPool pool(4);
    auto result_future = pool.enqueue(func);
    //    // get result from future
    //    int result = result_future.get();
    //    auto future = std::async(func);
  }
};

class LooperExecutor : public AbstractExecutor {
private:
  std::condition_variable queue_condition;
  std::mutex queue_lock;
  std::queue<std::function<void()>> executable_queue;

  std::atomic<bool> is_active;
  std::thread work_thread;

  void run_loop() {
    while (is_active.load(std::memory_order_relaxed) || !executable_queue.empty()) {
      std::unique_lock lock(queue_lock);
      if (executable_queue.empty()) {
        queue_condition.wait(lock);
        if (executable_queue.empty()) {
          continue;
        }
      }
      auto func = executable_queue.front();
      executable_queue.pop();
      lock.unlock();

      func();
    }
  }

public:
  LooperExecutor() {
    is_active.store(true, std::memory_order_relaxed);
    work_thread = std::thread(&LooperExecutor::run_loop, this);
  }

  ~LooperExecutor() {
    shutdown(false);
    if (work_thread.joinable()) {
      work_thread.join();
    }
  }

  void Execute(std::function<void()> &&func) override {
    std::unique_lock lock(queue_lock);
    if (is_active.load(std::memory_order_relaxed)) {
      executable_queue.push(func);
      lock.unlock();
      queue_condition.notify_one();
    }
  }

  void shutdown(bool wait_for_complete = true) {
    is_active.store(false, std::memory_order_relaxed);
    if (!wait_for_complete) {
      // clear queue.
      std::unique_lock lock(queue_lock);
      decltype(executable_queue) empty_queue;
      std::swap(executable_queue, empty_queue);
      lock.unlock();
    }

    queue_condition.notify_all();
  }
};

class SharedLooperExecutor : public AbstractExecutor {
public:
  void Execute(std::function<void()> &&func) override {
    static LooperExecutor sharedLooperExecutor;
    sharedLooperExecutor.Execute(std::move(func));
  }
};

#endif//RENDU_TASK_EXECUTOR_H
