/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_ASYNC_THREAD_HPP
#define RENDU_ASYNC_THREAD_HPP

#include "async_define.h"
#include <functional>
#include <memory>
#include <pthread.h>
#include "count_down_latch.h"

ASYNC_NAMESPACE_BEGIN

class Thread : NonCopyable {
 public:
  typedef std::function<void()> ThreadFunc;

  explicit Thread(ThreadFunc, const STRING &name = STRING());
  // FIXME: make it movable in C++11
  ~Thread();

  void start();
  int join(); // return pthread_join()

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const STRING &name() const { return name_; }

  static int numCreated() { return numCreated_; }

 private:
  void setDefaultName();

  bool started_;
  bool joined_;
  pthread_t pthreadId_;
  pid_t tid_;
  ThreadFunc func_;
  STRING name_;
  CountDownLatch latch_;

  static std::atomic<int> numCreated_;
};

ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_THREAD_HPP
