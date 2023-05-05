/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_HOST_H
#define RENDU_HOST_H

#include "adapter/base/process.h"
#include "adapter/ecs/entity_pool.h"
#include "adapter/ecs/system_pool.h"
#include "fwd/core_fwd.h"

namespace rendu {

    class Host{
    public:
        EntityPool m_entity_pool;
    private:
        SystemPool m_system_pool;
    public:
        template<typename Plugin>
        Host &AddPlugin() {
            Plugin plugin;
            RD_INFO("Plugin [{}] mount begin ... ",typeid(plugin).name());
            plugin.Mount(*this);
            RD_INFO("Plugin [{}] mount end ! ",typeid(plugin).name());
            return *this;
        }

        Host &AddSystem(std::shared_ptr<System> system);

        system_map &GetSystems(system_type systemType);
    };
}

#endif //RENDU_HOST_H
