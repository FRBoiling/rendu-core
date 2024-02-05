/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_THREAD_THREAD_HPP
#define RENDU_THREAD_THREAD_HPP

#include "thread_define.h"
#include <chrono>
#include <functional>
#include <thread>

THREAD_NAMESPACE_BEGIN

class SynchronizationContext;

class Thread : public NonCopyable {
public:
  Thread() = default;

  template<class _Fp, class... _Args>
  Thread(_Fp &&__f, _Args &&...__args)
      : thread_(std::make_unique<std::thread>(std::forward<_Fp>(__f), std::forward<_Args>(__args)...)) {}

  ~Thread() {
    if (IsAlive()) {
      Join();
    }
  }

  void Abort() {
    if (IsAlive()) {
      std::cout << "Thread is running, can't abort" << std::endl;
      return;
    }
  }

  bool IsAlive() const {
    return thread_ && thread_->joinable();
  }

  void Join() {
    if (IsAlive()) {
      thread_->join();
    }
  }

  void Detach() {
    if (IsAlive()) {
      thread_->detach();
    }
  }

  template<typename Callable, typename... Args>
  static Thread *Create(Callable &&func, Args &&...args) {
    return new Thread(std::forward<Callable>(func), std::forward<Args>(args)...);
  }

  template<typename _Rep, typename _Period>
  static auto Sleep(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    std::this_thread::sleep_for(duration);
  }

  static std::thread::id CurrentThread() {
    return std::this_thread::get_id();
  }

private:
  std::unique_ptr<std::thread> thread_;
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_HPP
