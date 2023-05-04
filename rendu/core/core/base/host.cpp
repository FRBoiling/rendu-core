/*
* Created by boil on 2023/5/3.
*/

#include "host.h"
using namespace rendu;

Host &Host::AddSystem(std::shared_ptr<System> system) {
    RD_INFO("AddSystem {}  ",typeid(system->GetClassType()).name());
    m_system_pool.AddSystem(system);
    return *this;
}

system_map &Host::GetSystems(system_type systemType) {
    return m_system_pool.GetSystems(systemType);
}
