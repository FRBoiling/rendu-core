/*
* Created by boil on 2023/9/21.
*/

#include "logger_system.h"
#include "world/world.h"
#include "options.pb.h"

CORE_NAMESPACE_BEGIN

    void LoggerSystem::Awake() {
//      auto entity = World::Instance().GetEntity();
//      auto options = m_entityPool->try_get<proto::core::Options>(entity);
//      if (options == nullptr) {
//        RD_ERROR("LoggerSystem::Awake error, options == nullptr !");
//        return;
//      }
//      m_setting = &m_entityPool->emplace<proto::core::LogSetting>(entity);
//      m_setting->set_level(options->loglevel());
//      m_setting->set_path("logs/");
//      m_setting->set_perheader(proto::core::AppType_descriptor()->FindValueByNumber(options->apptype())->name());
//      RD_LOGGER_INIT(m_setting->path(), m_setting->perheader(), "", (Logger::LogLevel) m_setting->level());
    }

CORE_NAMESPACE_END
