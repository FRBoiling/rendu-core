/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CMD_IO_SYSTEM_H_
#define RENDU_CMD_IO_SYSTEM_H_

#include "adapter/ecs/system.h"

namespace rendu {

    class CmdIoUpdateSystem final : public BaseSystem {
    public:
        explicit CmdIoUpdateSystem();
    public:
        void Run(Entity &entity) override;
    };

}

#endif //RENDU_CMD_IO_SYSTEM_H_
