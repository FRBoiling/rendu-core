/*
* Created by boil on 2024/1/21.
*/

#ifndef RENDU_ASYNC_THREAD_POOL_H
#define RENDU_ASYNC_THREAD_POOL_H

#include "async_define.h"
#include "thread.h"

#include <atomic>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

ASYNC_NAMESPACE_BEGIN

class ThreadPool : NonCopyable
{
 public:
  typedef std::function<void ()> Task;

  explicit ThreadPool(const STRING& nameArg = STRING("ThreadPool"));
  ~ThreadPool();

  // Must be called before start().
  void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
  void setThreadInitCallback(const Task& cb)
  { threadInitCallback_ = cb; }

  void start(int numThreads);
  void stop();

  const STRING& name() const
  { return name_; }

  size_t queueSize() const;

  template<class _Fp, class... _Args,
      typename TResult = typename std::invoke_result_t<typename std::decay<_Fp>::type, typename std::decay<_Args>::type...>>
  auto Run(_Fp &&f, _Args &&...args) -> std::future<TResult> {
    auto task = std::make_shared<std::packaged_task<TResult()>>(
        std::bind(std::forward<_Fp>(f), std::forward<_Args>(args)...));
    std::future<TResult> res = task->get_future();
    {
      if (threads_.empty()) {
         (*task)();
      } else {
        MutexLockGuard lock(mutex_);
        while (isFull() && running_) {
          notFull_.wait();
        }
        if (!running_) return res;
        assert(!isFull());

        queue_.push_back([task]() { (*task)(); });
        notEmpty_.notify();
      }
    }
    return res;
  }

 private:
  bool isFull() const REQUIRES(mutex_);
  void runInThread();
  Task take();

  mutable MutexLock mutex_;
  Condition notEmpty_ GUARDED_BY(mutex_);
  Condition notFull_ GUARDED_BY(mutex_);
  STRING name_;
  Task threadInitCallback_;
  std::vector<std::unique_ptr<Thread>> threads_;
  std::deque<Task> queue_ GUARDED_BY(mutex_);
  size_t maxQueueSize_;
  bool running_;
};

extern ThreadPool global_thread_pool;

ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_THREAD_POOL_H
