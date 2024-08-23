/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_FIBER_H
#define RENDU_FIBER_H

#include "core_define.h"

#include "ecs/i_update_system.h"
#include "ecs/i_late_update_system.h"
#include "ecs/scene.h"
#include "entity_system.h"
#include "mailboxes.h"
#include "world/actor/actor_id.h"
#include "world/log/log_invoker.h"
#include "world/log/i_log.h"

RD_NAMESPACE_BEGIN
// 定义Fiber类
class Fiber : public IDisposable {
  public:
    static Fiber *Instance; // 静态成员变量，线程静态

    Fiber(int id, int zone, int sceneType, const std::string &name)
      : id(id), zone(zone), entitySystem(new EntitySystem()), mailboxes(new Mailboxes()),
        threadSynchronizationContext(new ThreadSynchronizationContext()), log(nullptr),
        root(nullptr), isDisposed(false) {
      LogInvoker logInvoker;
      logInvoker.Fiber = id;
      //    logInvoker.Process = Options::Instance.Process;
      //    logInvoker.SceneName = SceneTypeSingleton::Instance.GetSceneName(sceneType);
      //    log = EventSystem::Instance.Invoke<LogInvoker, ILog>(logInvoker);

      root = new Scene(this, sceneType, 1, id, name);
    }

    ~Fiber() {
      Dispose();
    }

  public:
    void Update() {
      try {
        entitySystem->Publish(new UpdateEvent());
      } catch (const std::exception &e) {
        log->Error(e);
      }
    }

    void LateUpdate() {
      try {
        entitySystem->Publish(new LateUpdateEvent());
        FrameFinishUpdate();
        threadSynchronizationContext->Update();
      } catch (const std::exception &e) {
        log->Error(e);
      }
    }

    TaskRef WaitFrameFinish() {
      TaskPtr task = new Task();
      //    TaskRef task = Task::Create(true);
      //    std::lock_guard<std::mutex> lock(frameFinishTasksMutex);
      //    frameFinishTasks.push(task);
      return *task;
    }

    void Dispose() {
      if (isDisposed) {
        return;
      }
      isDisposed = true;
      root->Dispose();
    }

  private:
    void FrameFinishUpdate() {
      std::lock_guard<std::mutex> lock(frameFinishTasksMutex);
      while (!frameFinishTasks.empty()) {
        TaskRef task = frameFinishTasks.front();
        frameFinishTasks.pop();
        //      task.SetResult();
      }
    }

    int id;
    int zone;
    std::unique_ptr<EntitySystem> entitySystem;
    std::unique_ptr<Mailboxes> mailboxes;
    std::unique_ptr<ThreadSynchronizationContext> threadSynchronizationContext;
    ILog *log;
    Scene *root;
    bool isDisposed;
    std::mutex frameFinishTasksMutex;
    std::queue<Task> frameFinishTasks;
};

RD_NAMESPACE_END

#endif//RENDU_FIBER_H
