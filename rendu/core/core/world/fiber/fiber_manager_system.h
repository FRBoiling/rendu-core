/*
* Created by boil on 2023/9/22.
*/

#ifndef RENDU_FIBER_MANAGER_SYSTEM_H
#define RENDU_FIBER_MANAGER_SYSTEM_H

#include "base/utils/singleton.h"
#include "common/scheduler.h"
#include "concurrent/concurrent_map.h"
#include "entity/component_system.h"
#include "fiber/fiber.h"
#include "fiber_fwd.h"
#include "fiber_pool.h"
#include "main_thread_scheduler.h"
#include "task/task.h"

RD_NAMESPACE_BEGIN

    enum class SchedulerType {
      Main,
      Thread,
      ThreadPool,
    };

    class FiberManagerSystem
        : public Singleton<FiberManagerSystem>,
          public ComponentSystem<FiberPool>,
          public SystemAwake,
          public SystemUpdate,
          public SystemLateUpdate {

    public:
      void Awake() override;

      void Update() override;

      void LateUpdate() override;

      Task<int> Create(SchedulerType schedulerType, int fiberId, int zone, SceneType sceneType, std::string &name);

    public:
      Fiber *Get(int id);

      bool IsDisposed();

    private:
      int idGenerator = 10000000; // 10000000以下为保留的用于StartSceneConfig的fiber id, 1个区配置1000个纤程，可以配置10000个区
      MainThreadScheduler *m_mainThreadScheduler;
      ConcurrentMap<int, Fiber> m_fibers;
      Scheduler *m_schedulers[3];
    };

RD_NAMESPACE_END

#endif //RENDU_FIBER_MANAGER_SYSTEM_H
