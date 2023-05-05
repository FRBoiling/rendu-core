/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CMD_IO_SYSTEM_H_
#define RENDU_CMD_IO_SYSTEM_H_

#include "adapter/ecs/system.h"

namespace rendu {

    class CmdIoUpdateSystem final : public System {
    public:
        explicit CmdIoUpdateSystem();

    private:
    public:
        void Run(EntityPool &registry) override;

        SystemType GetSystemType() override;
    };

}

#endif //RENDU_CMD_IO_SYSTEM_H_
