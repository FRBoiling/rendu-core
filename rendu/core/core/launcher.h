/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_CORE_LAUNCHER_H_
#define RENDU_CORE_LAUNCHER_H_

#include "base/host.h"
#include "io/cmd_io_plugin.h"

namespace rendu {

    class Launcher {
    public:
        static int Run() {

            cmd_io_plugin::config config = {
                    .win_title = "rendu core",
                    .cap_fps = 60
            };

            cmd_io_plugin io(config);
            host()
                    .add_plugin(io)
                    .run();

            return 0;
        }
    };

}
#endif //RENDU_CORE_LAUNCHER_H_
