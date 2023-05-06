/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_SYSTEM_H
#define RENDU_SYSTEM_H

#include "entity_pool.h"

namespace rendu {

    enum class SystemType{
        Init,
        LateInit,
        Update,
        LateUpdate,
    };

    class System {
    public:
        System *GetClassType() { return this; }
        virtual SystemType GetSystemType() = 0;
        virtual void Run(Entity &entity) = 0;
    };

}

#endif //RENDU_SYSTEM_H
