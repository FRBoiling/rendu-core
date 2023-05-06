/*
* Created by boil on 2023/5/3.
*/

#include "host.h"

using namespace rendu;



Host &Host::AddSystem(std::shared_ptr<System> system) {
    m_system_pool.AddSystem(system);
    RD_INFO("AddSystem {} success !",typeid(system->GetClassType()).name());
    return *this;
}

Systems &Host::GetSystems(system_type systemType) {
    return m_system_pool.GetSystems(systemType);
}


