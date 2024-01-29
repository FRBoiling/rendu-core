/*
* Created by boil on 2023/9/26.
*/

#include "fiber.h"
#include "async.hpp"
#include "entity/scene.h"

RD_NAMESPACE_BEGIN

    Fiber::Fiber() = default;

    Fiber::Fiber(int id, int zone, rendu::SceneType sceneType, std::string &name)
        : m_id(id), m_zone(zone), m_name(name) {
      m_root = new Scene(*this, m_id, 1, sceneType, m_name);
    }

    Fiber *Fiber::Instance = nullptr;

    bool Fiber::IsDisposed() {
      return false;
    }

    ThreadSynchronizationContext &Fiber::GetThreadSynchronizationContext() {
      return m_threadSynchronizationContext;
    }

    void Fiber::Update() {
      try {
        m_entitySystem.Update();
      }
      catch (std::exception &e) {
        RD_CRITICAL("{}", e.what());
      }
    }

    void Fiber::LateUpdate() {
      try {
        m_entitySystem.LateUpdate();
        FrameFinishUpdate();
        m_threadSynchronizationContext.Update();
      }
      catch (std::exception &e) {
        RD_CRITICAL("{}", e.what());
      }
    }

    void Fiber::Dispose() {
      if (m_isDisposed) {
        return;
      }
      m_isDisposed = true;
      delete m_root;
      m_root = nullptr;
    }

    void Fiber::FrameFinishUpdate() {
      while (!m_frameFinishTasks.empty()) {
        auto task = m_frameFinishTasks.Dequeue();
//        task.SetResult();  //TODO:BOIL
      }
    }


RD_NAMESPACE_END
