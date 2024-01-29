/*
* Created by boil on 2023/9/28.
*/

#include "thread_scheduler.h"
#include "async/thread/thread_helper.h"
#include "fiber_manager_system.h"

RD_NAMESPACE_BEGIN

    ThreadScheduler::ThreadScheduler(FiberManagerSystem *fiberManagerSystem)
        : m_fiberManagerSystem(fiberManagerSystem),
        m_threads(new ConcurrentMap<int, Thread *>()) {
    }


    ThreadScheduler::~ThreadScheduler() {
      Dispose();
    }

    void ThreadScheduler::Add(int fiberId) {
      Thread *thread = Thread::Create(&ThreadScheduler::Loop, this, fiberId);
      m_threads->TryAdd(fiberId, thread);
    }

    void ThreadScheduler::Loop(int fiberId) {
      Fiber *fiber = m_fiberManagerSystem->Get(fiberId);
      Fiber::Instance = fiber;
      SynchronizationContext::SetSynchronizationContext(&fiber->GetThreadSynchronizationContext());

      while (true) {
        if (m_fiberManagerSystem->IsDisposed()) {
          return;
        }
        fiber = m_fiberManagerSystem->Get(fiberId);
        if (fiber == nullptr) {
          Thread *thread = nullptr;
          m_threads->Remove(fiberId, thread);
          return;
        }

        if (fiber->IsDisposed()) {
          Thread *thread = nullptr;
          m_threads->Remove(fiberId, thread);
          return;
        }

        fiber->Update();
        fiber->LateUpdate();
        Thread::Sleep(1000);
      }

    }

    void ThreadScheduler::Dispose() {
//      std::for_each(m_threads->begin(), m_threads->end(), [](Thread *thread) {
//        thread->join();
//        delete thread;
//      });
//      m_threads->clear();
//      delete m_threads;
    }


RD_NAMESPACE_END
