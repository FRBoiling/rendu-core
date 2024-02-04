/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_THREAD_THREAD_HPP
#define RENDU_THREAD_THREAD_HPP

#include "count_down_latch.h"
#include "thread_define.h"
#include <functional>
#include <memory>
#include <pthread.h>

THREAD_NAMESPACE_BEGIN
class SynchronizationContext;

class Thread : NonCopyable {
public:
  typedef std::function<void()> ThreadFunc;

  explicit Thread(ThreadFunc, const STRING &name = STRING());
  // FIXME: make it movable in C++11
  ~Thread();

  void Start();
  int Join();// return pthread_join()

  bool Started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t GetTid() const { return tid_; }
  const STRING &GetName() const { return name_; }
  void SetSynchronizationContext(SynchronizationContext* context);

  static int GetNumCreated() { return numCreated_; }

  template<class _Fp, class... _Args>
  static Thread * Create(_Fp &&__f, _Args &&...__args){

  }

  template<typename _Rep, typename _Period>
  static auto Sleep(std::chrono::duration<_Rep, _Period> &&duration) noexcept {
    return ;
  }

  static Thread* GetCurrentThread();

  static INT32 GetProcessorCount();

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

  THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_HPP
