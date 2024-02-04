/*
* Created by boil on 2023/9/28.
*/

#ifndef RENDU_THREAD_SCHEDULER_H
#define RENDU_THREAD_SCHEDULER_H

#include "i_scheduler.h"
#include "fiber_fwd.h"

CORE_NAMESPACE_BEGIN

    class ThreadScheduler : public IScheduler{

    public:
      explicit ThreadScheduler(FiberManagerSystem *fiberManagerSystem);
      ~ThreadScheduler() ;

    public:
      void Add(int fiber_id) override;
      void Dispose();

    private:
      void Loop(int fiber_id);

    private:
      ConcurrentMap<int, Thread*> m_threads;
      FiberManagerSystem *m_fiberManagerSystem;

    };

CORE_NAMESPACE_END

#endif //RENDU_THREAD_SCHEDULER_H
