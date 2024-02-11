#ifndef RENDU_CORE_THREAD_SYNCHRONIZATION_CONTEXT_H
#define RENDU_CORE_THREAD_SYNCHRONIZATION_CONTEXT_H

#include "core_define.h"
#include <future>

CORE_NAMESPACE_BEGIN

class ThreadSynchronizationContext: public SynchronizationContext{

public:
  using Action = std::function<Task<void>()>;

  ThreadSynchronizationContext() : stop_requested(false), THREAD_operations(0), th(&ThreadSynchronizationContext::Update, this) {}

  ~ThreadSynchronizationContext() {
    {
      std::unique_lock<std::mutex> locker(mtx);
      stop_requested = true;
      cv.notify_all();
    }
    th.join();
  }

  void Post(Action action) {
    std::unique_lock<std::mutex> locker(mtx);
    tasks.push(action);
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

public:
  void Update() {
    std::unique_lock<std::mutex> locker(mtx);
    cv.wait(locker, [&]() {
      return !tasks.empty() || (THREAD_operations == 0 && stop_requested);
    });
    if (THREAD_operations == 0 && stop_requested) {
      return;
    }
    auto task = std::move(tasks.front());
    tasks.pop();
    locker.unlock();
    task();
  }

public:
  std::queue<Action> tasks;
  std::mutex mtx;
  std::condition_variable cv;
  std::thread th;
  bool stop_requested;
  int THREAD_operations;
};

CORE_NAMESPACE_END

#endif//RENDU_CORE_THREAD_SYNCHRONIZATION_CONTEXT_H