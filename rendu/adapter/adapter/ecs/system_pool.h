/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_SYSTEM_POOL_H
#define RENDU_SYSTEM_POOL_H

#include "system.h"

namespace rendu {

    using system_type = SystemType;
    using Systems = std::vector<std::shared_ptr<BaseSystem>>;

    class SystemPool {
    private:
        std::map<system_type,Systems> m_systems;
    public:
        void AddSystem(std::shared_ptr<BaseSystem> &system);
        Systems& GetSystems(system_type systemType);
    };

}
#endif //RENDU_SYSTEM_POOL_H
