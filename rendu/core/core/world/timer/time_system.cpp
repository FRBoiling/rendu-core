/*
* Created by boil on 2023/9/21.
*/

#include "time_system.h"
#include "world/world.h"

RD_NAMESPACE_BEGIN

    void TimeSystem::Awake() {
      auto entity = World::Instance().GetEntity();
      m_dateTime = &m_entityPool->emplace<DateTime>(entity, DateTime::GetCurrentDateTime());
    }

    void TimeSystem::Update() {
      // 赋值long型是原子操作，线程安全
      FrameTime = ClientNow();
    }

    time_t TimeSystem::ClientNow() {
      return 0;
    }

RD_NAMESPACE_END


