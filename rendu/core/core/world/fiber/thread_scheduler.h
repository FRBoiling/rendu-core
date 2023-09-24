/*
* Created by boil on 2023/9/28.
*/

#ifndef RENDU_THREAD_SCHEDULER_H
#define RENDU_THREAD_SCHEDULER_H

#include "fiber_fwd.h"
#include "base/scheduler.h"
#include "thread/thread.h"
#include "concurrent/concurrent_map.h"

RD_NAMESPACE_BEGIN

    class ThreadScheduler : public Scheduler{

    public:
      explicit ThreadScheduler(FiberManagerSystem *fiberManagerSystem);
      ~ThreadScheduler() ;

    public:
      void Add(int fiberId) override;
      void Dispose();

    private:
      void Loop(int fiberId);

    private:
      ConcurrentMap<int, Thread*>* m_threads;
      FiberManagerSystem *m_fiberManagerSystem;

    };

RD_NAMESPACE_END

#endif //RENDU_THREAD_SCHEDULER_H
