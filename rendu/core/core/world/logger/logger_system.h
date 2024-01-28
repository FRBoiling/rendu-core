/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_LOGGER_SYSTEM_H
#define RENDU_LOGGER_SYSTEM_H

#include "base/utils/singleton.h"
#include "core_define.h"
#include "entity/component_system.h"
#include "logger.pb.h"

namespace rendu {

    class LoggerSystem
        : public Singleton<LoggerSystem>,
          public ComponentSystem<proto::core::LogSetting>,
          public SystemAwake {
    public:
      void Awake() override;

    private:
      proto::core::LogSetting *m_setting{};
    };

}//namespace rendu

#endif //RENDU_LOGGER_SYSTEM_H
