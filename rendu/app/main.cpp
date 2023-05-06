/*
* Created by boil on 2022/12/31.
*/

#include "core/launcher.h"
#include "io/cmd_io_plugin.h"
#include "env/env_plugin.h"
#include "config/config_plugin.h"

using namespace rendu;

int main(int argc, char **argv) {
    Launcher::GetInst().AddPlugin<EnvPlugin>();
    Launcher::GetInst().AddPlugin<ConfigPlugin>();
//    launcher.AddPlugin<CmdIoPlugin>();
    Launcher::GetInst().Run();
    return 0;
}
