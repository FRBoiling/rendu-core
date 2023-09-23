/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_TIME_SYSTEM_H
#define RENDU_TIME_SYSTEM_H

#include "define.h"
#include <ctime>
#include "time_info.h"
#include "entity/component_system.h"
#include "base/singleton.h"

RD_NAMESPACE_BEGIN

    class TimeSystem
        : public Singleton<TimeSystem>,
          public ComponentSystem<DateTime>,
          public SystemAwake,
          public SystemUpdate {
    public:
      void Awake() override;

      void Update() override;

    private:
      time_t FrameTime;

      time_t ClientNow();

    private:
      DateTime* m_dateTime;
    };


RD_NAMESPACE_END


#endif //RENDU_TIME_SYSTEM_H
