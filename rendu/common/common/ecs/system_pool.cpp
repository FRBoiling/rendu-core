/*
* Created by boil on 2023/5/2.
*/

#include "system_pool.h"

COMMON_NAMESPACE_BEGIN

    void SystemPool::AddSystem(std::shared_ptr<BaseSystem> &system) {
      auto system_type = system->GetSystemType();
      m_systems[system_type].push_back(system);
    }

    Systems &SystemPool::GetSystems(system_type systemType) {
      return m_systems[systemType];
    }

COMMON_NAMESPACE_END