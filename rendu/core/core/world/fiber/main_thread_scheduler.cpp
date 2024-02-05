/*
* Created by boil on 2023/9/27.
*/

#include "main_thread_scheduler.h"
#include "fiber_manager_system.h"
#include "fiber/fiber.h"

CORE_NAMESPACE_BEGIN

    MainThreadScheduler::MainThreadScheduler(FiberManagerSystem *fiberManagerSystem):m_fiberManagerSystem(fiberManagerSystem) {
      SynchronizationContext::SetSynchronizationContext(m_threadSynchronizationContext);
    }

    void MainThreadScheduler::Add(int fiberId) {
      m_idQueue.Enqueue(fiberId);
    }

    void MainThreadScheduler::Update() {
      SynchronizationContext::SetSynchronizationContext(m_threadSynchronizationContext);
      m_threadSynchronizationContext->Update();

      int count = m_idQueue.Size();
      while (count-- > 0) {
        int id = 0;
        if (!m_idQueue.TryDequeue(id)) {
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
        SynchronizationContext::SetSynchronizationContext(fiber->GetThreadSynchronizationContext());
        fiber->Update();
        Fiber::Instance = nullptr;
        m_idQueue.Enqueue(id);
      }
    }

    void MainThreadScheduler::LateUpdate() {
      int count = m_idQueue.Size();
      while (count-- > 0) {
        int id = 0;
        if (!m_idQueue.TryDequeue(id)) {
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
        SynchronizationContext::SetSynchronizationContext(fiber->GetThreadSynchronizationContext());
        fiber->LateUpdate();
        Fiber::Instance = nullptr;
        m_idQueue.Enqueue(id);
      }

      while (m_addIds.Size() > 0) {
        int addId = 0;
        m_addIds.TryDequeue(addId);
        m_idQueue.Enqueue(addId);
      }
    }

CORE_NAMESPACE_END


