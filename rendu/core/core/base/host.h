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
    private:
        EntityPool m_entity_pool;
        SystemPool m_system_pool;
    public:
        Host &AddSystem(std::shared_ptr<System> system) {
            RD_INFO("AddSystem {}  ",typeid(system->GetClassType()).name());
            m_system_pool.AddSystem(system);
            return *this;
        }

        template<typename T>
        Host &AddPlugin() {
            T plugin;
            RD_INFO("Plugin [{}] mount begin ... ",typeid(plugin).name());
            plugin.Mount(*this);
            RD_INFO("Plugin [{}] mount end ! ",typeid(plugin).name());
            return *this;
        }


    };

}

#endif //RENDU_CORE_BASE_HOST_H_
