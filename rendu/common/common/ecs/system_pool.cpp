/*
* Created by boil on 2023/5/2.
*/

#include "system_pool.h"

using namespace rendu;


void SystemPool::AddSystem(std::shared_ptr<System> &system) {
    auto system_type = system->GetSystemType();
    auto class_name = typeid(system->GetClassType()).name();
    m_systems[system_type][class_name] = system;
}

void SystemPool::RemoveSystem(std::shared_ptr<System> &system) {
    auto system_type = system->GetSystemType();
    auto class_name = typeid(system->GetClassType()).name();
    auto systems_itr = m_systems.find(system_type);
    if (systems_itr == m_systems.end()){
        return;
    }
    auto &systems = systems_itr->second;
    auto system_itr = systems.find(class_name);
    if (system_itr == systems.end()){
        return;
    }
    systems.erase(class_name);
    if (!systems.empty()){
        return;
    }
    m_systems.erase(system_type);
}
