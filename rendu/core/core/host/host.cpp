/*
* Created by boil on 2023/5/3.
*/

#include "host.h"

using namespace rendu;



Host &Host::AddSystem(std::shared_ptr<BaseSystem> system) {
    m_system_pool.AddSystem(system);
    RD_INFO("AddSystem {} success !",system->ToString());
    return *this;
}

Systems &Host::GetSystems(system_type systemType) {
    return m_system_pool.GetSystems(systemType);
}


