/*
* Created by boil on 2023/9/28.
*/

#ifndef RENDU_THREAD_POOL_SCHEDULER_H
#define RENDU_THREAD_POOL_SCHEDULER_H

#include "fiber_fwd.h"
#include "base/scheduler.h"
#include "thread/thread.h"
#include "concurrent/concurrent_queue.h"

RD_NAMESPACE_BEGIN

    class ThreadPoolScheduler : public Scheduler {
    public:
      explicit ThreadPoolScheduler(FiberManagerSystem *fiberManagerSystem);

      ~ThreadPoolScheduler();

    public:
      void Run();
      void Add(int fiberId) override;

      void Dispose();

    private:
      void Loop(int fiberId);

    private:
      std::vector<Thread *> *m_threads{};
      ConcurrentQueue<int> m_idQueue;
      FiberManagerSystem *m_fiberManagerSystem{};
    };

RD_NAMESPACE_END

#endif //RENDU_THREAD_POOL_SCHEDULER_H
