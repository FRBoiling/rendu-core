/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_SYSTEM_POOL_H
#define RENDU_SYSTEM_POOL_H

#include "system.h"

namespace rendu {
    class SystemPool {
    private:
        std::map<SystemType, std::unordered_map<std::string, std::shared_ptr<System>>> m_systems;

    public:
        void AddSystem(std::shared_ptr<System> &system);

        void RemoveSystem(std::shared_ptr<System> &system);
    };

}
#endif //RENDU_SYSTEM_POOL_H
