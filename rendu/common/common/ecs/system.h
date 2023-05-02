/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_SYSTEM_H
#define RENDU_SYSTEM_H

#include "system_type.h"
#include "entity_pool.h"

namespace rendu {

    class System {
    public:
        System *GetClassType() { return this; }
        virtual SystemType GetSystemType() = 0;
        virtual void Run(EntityPool &registry) = 0;
    };

}

#endif //RENDU_SYSTEM_H
