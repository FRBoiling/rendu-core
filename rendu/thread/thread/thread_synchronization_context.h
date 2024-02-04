/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H
#define RENDU_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H

#include "thread_define.h"
#include <future>

THREAD_NAMESPACE_BEGIN

class ThreadSynchronizationContext {
public:
  ThreadSynchronizationContext() : stop_requested(false), THREAD_operations(0), th(&ThreadSynchronizationContext::Update, this) {}

  ~ThreadSynchronizationContext() {
    {
      std::unique_lock<std::mutex> locker(mtx);
      stop_requested = true;
      cv.notify_all();
    }
    th.join();
  }

  template<typename Task>
  void Post(Task task) {
    std::unique_lock<std::mutex> locker(mtx);
    tasks.push(std::move(task));
    cv.notify_all();
  }

  template<typename Callable>
  auto Send(Callable &&callable) {
    if (std::this_thread::get_id() == th.get_id()) {
      return callable();
    } else {
      std::future<decltype(callable())> result;
      std::packaged_task<decltype(callable())()> task(std::forward<Callable>(callable));
      result = task.get_future();
      Post(std::move(task));
      return result.get();
    }
  }

  void OperationStarted() {
    std::unique_lock<std::mutex> locker(mtx);
    THREAD_operations++;
  }

  void OperationCompleted() {
    std::unique_lock<std::mutex> locker(mtx);
    THREAD_operations--;
    if (THREAD_operations == 0) {
      cv.notify_all();
    }
  }

  void Update() {
    std::unique_lock<std::mutex> locker(mtx);
    cv.wait(locker, [&]() {
      return !tasks.empty() || (THREAD_operations == 0 && stop_requested);
    });
    if (THREAD_operations == 0 && stop_requested) {
      return;
    }
    std::packaged_task<void()> task = std::move(tasks.front());
    tasks.pop();
    locker.unlock();
    task();
  }

  std::queue<std::packaged_task<void()>> tasks;
  std::mutex mtx;
  std::condition_variable cv;
  std::thread th;
  bool stop_requested;
  int THREAD_operations;
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_SYNCHRONIZATION_CONTEXT_H
