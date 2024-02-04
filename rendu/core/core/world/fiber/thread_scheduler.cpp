/*
* Created by boil on 2023/9/28.
*/

#include "thread_scheduler.h"
#include "fiber_manager_system.h"

CORE_NAMESPACE_BEGIN

    ThreadScheduler::ThreadScheduler(FiberManagerSystem *fiberManagerSystem)
        : m_fiberManagerSystem(fiberManagerSystem){
    }


    ThreadScheduler::~ThreadScheduler() {
      Dispose();
    }

    void ThreadScheduler::Add(int fiber_id) {
      Thread *thread = Thread::Create(&ThreadScheduler::Loop, this,fiber_id);
      m_threads.TryAdd(fiber_id, thread);
    }

    void ThreadScheduler::Loop(int fiber_id) {
      Fiber *fiber = m_fiberManagerSystem->Get(fiber_id);
      Fiber::Instance = fiber;
      SynchronizationContext::SetSynchronizationContext(fiber->GetThreadSynchronizationContext());

      while (true) {
        if (m_fiberManagerSystem->IsDisposed()) {
          return;
        }
        fiber = m_fiberManagerSystem->Get(fiber_id);
        if (fiber == nullptr) {
          m_threads.Remove(fiber_id);
          return;
        }

        if (fiber->IsDisposed()) {
          m_threads.Remove(fiber_id);
          return;
        }

        fiber->Update();
        fiber->LateUpdate();
        Thread::Sleep(1s);
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


CORE_NAMESPACE_END
