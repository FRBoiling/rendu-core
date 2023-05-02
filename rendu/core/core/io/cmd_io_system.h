/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_IO_CMD_IO_SYSTEM_H_
#define RENDU_CORE_IO_CMD_IO_SYSTEM_H_

#include "ecs/system.h"

namespace rendu {

    class CmdIoSystem final : public System {
    public:
        explicit CmdIoSystem();

    private:
    public:
        template<typename Component>
        void register_component(const std::string &display_name) {
//    m_editor.registerComponent<Component>(display_name);
        }

        void Run(EntityPool &registry) override;

        SystemType GetSystemType() override;
    };

}

#endif //RENDU_CORE_IO_CMD_IO_SYSTEM_H_
