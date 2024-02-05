/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_THREAD_THREAD_HPP
#define RENDU_THREAD_THREAD_HPP

#include "thread_define.h"
#include <thread>
#include <functional>
#include <chrono>

THREAD_NAMESPACE_BEGIN

class SynchronizationContext;

class Thread {
public:
  Thread() = default;

  template<typename Callable, typename... Args>
  explicit Thread(Callable&& func, Args&&... args)
      : thread_(std::forward<Callable>(func), std::forward<Args>(args)...) {}

  ~Thread() {
    // Avoid detaching threads, it may lead to resource leaks
    // Always join them in the destructor
    if (thread_.joinable()) {
      //      thread_.detach();
      thread_.join();
    }
  }

  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;
  Thread(Thread&&) = default;
  Thread& operator=(Thread&&) = default;

  template<typename Callable, typename... Args>
  void Start(Callable&& func, Args&&... args) {
    thread_ = std::thread(std::forward<Callable>(func), std::forward<Args>(args)...);
  }

  void Join() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void Abort() {
    //C++ does not support aborting threads in the same way C# does
    //suggest to use condition variables or atomics for signaling
  }

  bool IsAlive() const {
    // May need a more sophisticated mechanism depending on your specific use case
    return thread_.joinable();
  }

  static void Sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }

  static std::thread::id CurrentThread() {
    return std::this_thread::get_id();
  }

private:
  std::thread thread_;
};

  THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_HPP
