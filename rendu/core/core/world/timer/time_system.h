/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_TIME_SYSTEM_H
#define RENDU_TIME_SYSTEM_H

#include "define.h"
#include <ctime>
#include "entity/component_system.h"
#include "base/singleton.h"
#include "timer/date_time.h"

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
      DateTime* m_dateTime;
      uint64_t m_lastFrameTime;
    };


RD_NAMESPACE_END


#endif //RENDU_TIME_SYSTEM_H
