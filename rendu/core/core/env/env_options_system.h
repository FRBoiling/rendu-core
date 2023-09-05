/*
* Created by boil on 2023/9/5.
*/

#ifndef RENDU_CORE_ENV_OPTIONS_SYSTEM_H
#define RENDU_CORE_ENV_OPTIONS_SYSTEM_H

#include "adapter/ecs/system.h"

namespace rendu{

    class EnvOptionsInitSystem :public BaseInitSystem{
    public:
        std::string ToString() override{
            return "EnvOptionsInitSystem";
        }
    };

    class EnvOptionsUpdateSystem :public BaseUpdateSystem{
    public:
        std::string ToString() override{
            return "EnvOptionsUpdateSystem";
        }
    };
}



#endif //RENDU_CORE_ENV_OPTIONS_SYSTEM_H
