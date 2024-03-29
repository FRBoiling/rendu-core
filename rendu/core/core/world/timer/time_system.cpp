/*
* Created by boil on 2023/9/21.
*/

#include "time_system.h"
#include "world/world.h"
#include "base/thread/thread_helper.h"

RD_NAMESPACE_BEGIN

    void TimeSystem::Awake() {
      auto entity = World::Instance().GetEntity();
      m_dateTime = &m_entityPool->emplace<DateTime>(entity, DateTime::GetCurrentDateTime());
      m_lastFrameTime = m_dateTime->GetTimeStamp();
    }

    void TimeSystem::Update() {
      // 赋值long型是原子操作，线程安全
      auto newTime = DateTime::GetCurrentDateTime();
      m_dateTime->SetDateTime(newTime);
      if (newTime.GetTimeStamp() - m_lastFrameTime > 1000) {
        m_lastFrameTime = m_dateTime->GetTimeStamp();
        RD_INFO("当前时间 ：{}", m_dateTime->ToString());
      }
    }

RD_NAMESPACE_END


