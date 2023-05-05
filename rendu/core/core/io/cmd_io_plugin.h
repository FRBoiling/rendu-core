/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_IO_CMD_IO_PLUGIN_H_
#define RENDU_CORE_IO_CMD_IO_PLUGIN_H_

#include "base/plugin.h"

namespace rendu {

    class CmdIoPlugin final : public Plugin {
    public:
        explicit CmdIoPlugin() ;
    public:
        void Mount(Host &host) override ;
    };
}

#endif //RENDU_CORE_IO_CMD_IO_PLUGIN_H_
