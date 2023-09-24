/*
* Created by boil on 2023/9/22.
*/

#include "fiber_manager_system.h"
#include "thread_scheduler.h"
#include "thread_pool_scheduler.h"
#include "exception/throw_helper.h"
#include "task/task.h"
#include "fmt/core.h"
#include "task/task_completion_source.h"
#include "../event/event_system.h"
#include "fiber/fiber_init.h"
#include "logger/log.h"

RD_NAMESPACE_BEGIN

    void FiberManagerSystem::Awake() {
      m_mainThreadScheduler = new MainThreadScheduler(this);
      m_schedulers[(int) SchedulerType::Main] = m_mainThreadScheduler;
      m_schedulers[(int) SchedulerType::Thread] = new ThreadScheduler(this);
      m_schedulers[(int) SchedulerType::ThreadPool] = new ThreadPoolScheduler(this);
    }

    void FiberManagerSystem::Update() {
      m_mainThreadScheduler->Update();
    }

    void FiberManagerSystem::LateUpdate() {
      m_mainThreadScheduler->LateUpdate();
    }

    // 不允许外部调用，容易出现多线程问题, 只能通过消息通信，不允许直接获取其它Fiber引用
    Fiber *FiberManagerSystem::Get(int id) {
      Fiber *fiber = nullptr;
      m_fibers.TryGetValue(id, fiber);
      return fiber;
    }

    bool FiberManagerSystem::IsDisposed() {
      return false;
    }

    Task<int> FiberManagerSystem::Create(SchedulerType schedulerType, int fiberId, int zone, SceneType sceneType,
                                         std::string &name) {

      try {
        Fiber *fiber = new Fiber(fiberId, zone, sceneType, name);

        if (!m_fibers.TryAdd(fiberId, *fiber)) {
          ThrowHelper::EntityException(
              "same fiber already existed, if you remove, please await Remove then Create fiber! {fiberId}");
        }
        m_schedulers[(int) schedulerType]->Add(fiberId);

        TaskCompletionSource<bool> tcs;
        fiber->GetThreadSynchronizationContext().Post([sceneType,fiber,tcs]() -> Task<> {
          try {
            // 根据Fiber的SceneType分发Init,必须在Fiber线程中执行
            co_await EventSystem::Instance().Invoke<FiberInit*,void>((long)sceneType,  new FiberInit());
            tcs.SetResult(true);
          }
          catch (std::exception &e) {
            RD_CRITICAL("init fiber fail: {} {}", (long)sceneType, e.what());
          }
          co_return;
        });
        co_return fiberId;
      }
      catch (std::exception &e) {
        auto msg = fmt::format("create fiber error: {} {},{} ", fiberId, sceneType, e.what());
        ThrowHelper::EntityException(msg.c_str());
      }
      co_return 0;
    }


RD_NAMESPACE_END


