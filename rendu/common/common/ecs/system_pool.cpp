/*
* Created by boil on 2023/5/2.
*/

#include "system_pool.h"

using namespace rendu;


void SystemPool::AddSystem(std::shared_ptr<System> &system) {
    auto system_type = system->GetSystemType();
    auto system_key = typeid(system->GetClassType()).name();
    m_systems[system_type][system_key] = system;
}

void SystemPool::RemoveSystem(std::shared_ptr<System> &system) {
    auto system_type = system->GetSystemType();
    auto system_key = typeid(system->GetClassType()).name();
    auto systems_itr = m_systems.find(system_type);
    if (systems_itr == m_systems.end()){
        return;
    }
    auto &systems = systems_itr->second;
    auto system_itr = systems.find(system_key);
    if (system_itr == systems.end()){
        return;
    }
    systems.erase(system_key);
}

system_map &SystemPool::GetSystems(system_type systemType) {
    return m_systems[systemType];
}
