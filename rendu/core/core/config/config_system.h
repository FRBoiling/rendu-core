/*
* Created by boil on 2023/5/5.
*/

#ifndef RENDU_CONFIG_SYSTEM_H
#define RENDU_CONFIG_SYSTEM_H

#include "adapter/ecs/system.h"

namespace rendu{

    class ConfigInitSystem :public System{
    public:
        SystemType GetSystemType() override;

        void Run(EntityPool &registry) override;
    };

    class ConfigLaterInitSystem :public System{
    public:
        SystemType GetSystemType() override;

        void Run(EntityPool &registry) override;
    };

    class ConfigUpdateSystem :public System{
    public:
        SystemType GetSystemType() override;

        void Run(EntityPool &registry) override;
    };
}


#endif //RENDU_CONFIG_SYSTEM_H
