/*
* Created by boil on 2024/1/29.
*/

#include "thread.h"
#include "thread_helper.h"

ASYNC_NAMESPACE_BEGIN

void afterFork() {
  CurrentThread::t_cachedTid = 0;
  CurrentThread::t_threadName = "main";
  CurrentThread::tid();
  // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer {
 public:
  ThreadNameInitializer() {
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    pthread_atfork(NULL, NULL, &afterFork);
  }
};

ThreadNameInitializer init;

struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  STRING name_;
  pid_t *tid_;
  CountDownLatch *latch_;

  ThreadData(ThreadFunc func,
             const STRING &name,
             pid_t *tid,
             CountDownLatch *latch)
      : func_(std::move(func)),
        name_(name),
        tid_(tid),
        latch_(latch) {}

  void runInThread() {
    *tid_ = CurrentThread::tid();
    tid_ = NULL;
    latch_->countDown();
    latch_ = NULL;

    CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
//    ::prctl(PR_SET_NAME, CurrentThread::t_threadName);
    try {
      func_();
      CurrentThread::t_threadName = "finished";
    }
    catch (const Exception &ex) {
      CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      fprintf(stderr, "stack trace: %s\n", ex.StackTrace());
      abort();
    }
    catch (const std::exception &ex) {
      CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
      fprintf(stderr, "reason: %s\n", ex.what());
      abort();
    }
    catch (...) {
      CurrentThread::t_threadName = "crashed";
      fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
      throw; // rethrow
    }
  }
};

void *startThread(void *obj) {
  ThreadData *data = static_cast<ThreadData *>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

void CurrentThread::cacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = GetPid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

bool CurrentThread::isMainThread() {
  return tid() == GetPid();
}

void CurrentThread::sleepUsec(int64_t usec) {
  struct timespec ts = {0, 0};
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, NULL);
}

std::atomic<int> Thread::numCreated_ {0};

Thread::Thread(ThreadFunc func, const STRING &n)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(std::move(func)),
      name_(n),
      latch_(1) {
  setDefaultName();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthreadId_);
  }
}

void Thread::setDefaultName() {
  int num = numCreated_++;
  if (name_.empty()) {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::Start() {
  assert(!started_);
  started_ = true;
  // FIXME: move(func_)
  auto data = new ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
    started_ = false;
    delete data; // or no delete?
    RD_TRACE("Failed in pthread_create");
  } else {
    latch_.wait();
    assert(tid_ > 0);
  }
}

int Thread::Join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}

ASYNC_NAMESPACE_END