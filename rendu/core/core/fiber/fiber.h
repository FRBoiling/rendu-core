/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_FIBER_H
#define RENDU_FIBER_H

#include "actor/address.h"
#include "core_define.h"
#include "entity/scene.h"
#include "entity/scene_type.h"
#include "entity_system.h"
#include "mailboxes.h"
#include <queue>

RD_NAMESPACE_BEGIN


    class Fiber {

    public:
      Fiber();

      Fiber(int id, int zone, SceneType sceneType, std::string &name);

    public:
      bool IsDisposed();

      ThreadSynchronizationContext &GetThreadSynchronizationContext();

      void Update();

      void LateUpdate();

    public:
      static Fiber *Instance;
    public:
      void Dispose();

    private:
      int m_id{};
      int m_zone{};
      std::string m_name{};
      Scene *m_root{};
      Address m_address{};

      bool m_isDisposed{};

      EntitySystem m_entitySystem{};
      Mailboxes m_mailboxes{};
      ThreadSynchronizationContext m_threadSynchronizationContext{};

      Queue<Task<void>*> m_frameFinishTasks{};
    private:
      void FrameFinishUpdate();
    };



RD_NAMESPACE_END

#endif //RENDU_FIBER_H
