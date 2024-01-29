/*
* Created by boil on 2024/1/21.
*/

#include "thread_pool.h"

ASYNC_NAMESPACE_BEGIN

ThreadPool global_thread_pool;

ThreadPool::ThreadPool(const STRING &nameArg)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(nameArg),
      maxQueueSize_(0),
      running_(false) {
}

ThreadPool::~ThreadPool() {
  if (running_) {
    stop();
  }
}

void ThreadPool::start(INT32 numThreads) {
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(numThreads);
  for (int i = 0; i < numThreads; ++i) {
    char id[32];
    snprintf(id, sizeof id, "%d", i + 1);
    threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
    threads_[i]->Start();
  }
  if (numThreads == 0 && threadInitCallback_) {
    threadInitCallback_();
  }
}

void ThreadPool::stop() {
  {
    MutexLockGuard lock(mutex_);
    running_ = false;
    notEmpty_.notifyAll();
    notFull_.notifyAll();
  }
  for (auto &thr : threads_) {
    thr->Join();
  }
}

size_t ThreadPool::queueSize() const {
  MutexLockGuard lock(mutex_);
  return queue_.size();
}

ThreadPool::Task ThreadPool::take() {
  MutexLockGuard lock(mutex_);
  // always use a while-loop, due to spurious wakeup
  while (queue_.empty() && running_) {
    notEmpty_.wait();
  }
  Task task;
  if (!queue_.empty()) {
    task = queue_.front();
    queue_.pop_front();
    if (maxQueueSize_ > 0) {
      notFull_.notify();
    }
  }
  return task;
}

bool ThreadPool::isFull() const {
  mutex_.assertLocked();
  return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread() {
  try {
    if (threadInitCallback_) {
      threadInitCallback_();
    }
    while (running_) {
      Task task(take());
      if (task) {
        task();
      }
    }
  }
  catch (const Exception &ex) {
    RD_CRITICAL("exception caught in ThreadPool {}\n reason:{}\nstack trace:{}",
                name_.c_str(), ex.what(), ex.StackTrace());
    throw; // rethrow
  }
  catch (const std::exception &ex) {
    RD_CRITICAL("exception caught in ThreadPool {}\n reason:{}\nstack trace:{}",
                name_.c_str(), ex.what());
    throw; // rethrow
  }
  catch (...) {
    RD_CRITICAL("unknown exception caught in ThreadPool {}", name_.c_str());
    throw; // rethrow
  }
}

ASYNC_NAMESPACE_END