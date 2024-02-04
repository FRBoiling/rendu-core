/*
* Created by boil on 2023/9/28.
*/

#ifndef RENDU_THREAD_POOL_SCHEDULER_H
#define RENDU_THREAD_POOL_SCHEDULER_H

#include "fiber_fwd.h"
#include "i_scheduler.h"

CORE_NAMESPACE_BEGIN


    class ThreadPoolScheduler : public IScheduler {
    public:
      explicit ThreadPoolScheduler(FiberManagerSystem *fiberManagerSystem);

      ~ThreadPoolScheduler();

    public:
      void Add(int fiberId) override;

      void Dispose();
      void Run();

    private:
      void Loop(int fiberId);

    private:
      std::vector<Thread *> *m_threads{};
      ConcurrentQueue<int> m_idQueue;
      FiberManagerSystem *m_fiberManagerSystem{};
    };

CORE_NAMESPACE_END

#endif //RENDU_THREAD_POOL_SCHEDULER_H
