/*
* Created by boil on 2023/9/28.
*/

#include "thread_pool_scheduler.h"
#include "fiber_manager_system.h"
#include "base/thread/thread_helper.h"

RD_NAMESPACE_BEGIN

    ThreadPoolScheduler::ThreadPoolScheduler(FiberManagerSystem *fiberManagerSystem)
        : m_fiberManagerSystem(fiberManagerSystem),
          m_threads(new std::vector<Thread *>()) {
      Run();
    }

    ThreadPoolScheduler::~ThreadPoolScheduler() {
      Dispose();
    }

    void ThreadPoolScheduler::Add(int fiberId) {
      m_idQueue.Enqueue(fiberId);
    }

    void ThreadPoolScheduler::Loop(int fiberId) {
      while (true) {
        if (m_fiberManagerSystem->IsDisposed()) {
          return;
        }
        int id = 0;
        if (!m_idQueue.TryDequeue(id)) {
          Thread::Sleep(1000);
          continue;
        }

        Fiber *fiber = m_fiberManagerSystem->Get(id);
        if (fiber == nullptr) {
          continue;
        }

        if (fiber->IsDisposed()) {
          continue;
        }

        Fiber::Instance = fiber;
        SynchronizationContext::SetSynchronizationContext(&fiber->GetThreadSynchronizationContext());
        fiber->Update();
        fiber->LateUpdate();
        SynchronizationContext::SetSynchronizationContext(nullptr);
        Fiber::Instance = nullptr;
        m_idQueue.Enqueue(id);

        Thread::Sleep(1000);
      }
    }

    void ThreadPoolScheduler::Dispose() {
      std::for_each(m_threads->begin(), m_threads->end(), [](Thread *thread) {
        thread->join();
        delete thread;
      });
      m_threads->clear();
      delete m_threads;
    }

    void ThreadPoolScheduler::Run() {
      int threadCount = Environment::GetProcessorCount();
      m_threads = new std::vector<Thread *>(threadCount);
      for (int i = 0; i < threadCount; ++i) {
        Thread *thread = Thread::Create(&ThreadPoolScheduler::Loop, this, 0);
        m_threads->push_back(thread);
      }
    }


RD_NAMESPACE_END