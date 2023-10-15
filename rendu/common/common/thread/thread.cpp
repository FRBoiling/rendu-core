/*
* Created by boil on 2023/11/2.
*/

#include "thread.h"
#include "thread_data.h"

RD_NAMESPACE_BEGIN

  void* startThread(void* obj)
  {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
  }


  AtomicInt32 Thread::numCreated_;

  Thread::Thread(ThreadFunc func, const string& n)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(std::move(func)),
      name_(n),
      latch_(1)
  {
    setDefaultName();
  }

  Thread::~Thread()
  {
    if (started_ && !joined_)
    {
      pthread_detach(pthreadId_);
    }
  }

  void Thread::setDefaultName()
  {
    int num = numCreated_.incrementAndGet();
    if (name_.empty())
    {
      char buf[32];
      snprintf(buf, sizeof buf, "Thread%d", num);
      name_ = buf;
    }
  }

  void Thread::start()
  {
    assert(!started_);
    started_ = true;
    // FIXME: move(func_)
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthreadId_, NULL, &startThread, data))
    {
      started_ = false;
      delete data; // or no delete?
      assert(started_);
    }
    else
    {
      latch_.wait();
      assert(tid_ > 0);
    }
  }

  int Thread::join()
  {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
  }

RD_NAMESPACE_END