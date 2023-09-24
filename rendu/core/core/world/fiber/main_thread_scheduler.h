/*
* Created by boil on 2023/9/27.
*/

#ifndef RENDU_MAIN_THREAD_SCHEDULER_H
#define RENDU_MAIN_THREAD_SCHEDULER_H

#include "fiber_fwd.h"
#include "thread/thread_synchronization_context.h"
#include "base/scheduler.h"

RD_NAMESPACE_BEGIN

    class MainThreadScheduler: public Scheduler{
    public:
      explicit MainThreadScheduler(FiberManagerSystem* fiberSystem);

      void Add(int fiberId) override;

      void Update();

      void LateUpdate();

    private:
      ConcurrentQueue<int> m_idQueue;
      ConcurrentQueue<int> m_addIds;
      ThreadSynchronizationContext m_threadSynchronizationContext;

      FiberManagerSystem* m_fiberManagerSystem;

    };

RD_NAMESPACE_END

#endif //RENDU_MAIN_THREAD_SCHEDULER_H
