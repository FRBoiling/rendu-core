/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_SYSTEM_POOL_H
#define RENDU_SYSTEM_POOL_H

#include "system.h"

namespace rendu {

    using system_type = SystemType;
    using system_key = std::string;
    using system_map = std::unordered_map<system_key, std::shared_ptr<System>>;

    class SystemPool {
    private:
        std::map<system_type,system_map> m_systems;
    public:
        void AddSystem(std::shared_ptr<System> &system);

        void RemoveSystem(std::shared_ptr<System> &system);

        system_map& GetSystems(system_type systemType);

    };

}
#endif //RENDU_SYSTEM_POOL_H
