/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_BASE_HOST_H_
#define RENDU_CORE_BASE_HOST_H_

#include "common/base/process.h"
#include "common/ecs/entity_pool.h"
#include "common/ecs/system_pool.h"
#include "plugin.h"

namespace rendu {

    class Host : public Process<Host, std::uint32_t> {
    public:
        EntityPool m_entity_pool;
    private:
        SystemPool m_system_pool;
    public:
        template<typename T>
        Host &AddPlugin() {
            T plugin;
            RD_INFO("Plugin [{}] mount begin ... ",typeid(plugin).name());
            plugin.Mount(*this);
            RD_INFO("Plugin [{}] mount end ! ",typeid(plugin).name());
            return *this;
        }

        Host &AddSystem(std::shared_ptr<System> system);

        system_map &GetSystems(system_type systemType);


    };



}

#endif //RENDU_CORE_BASE_HOST_H_
