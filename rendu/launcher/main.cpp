/*
* Created by boil on 2022/12/31.
*/

#include "core/launcher.h"
#include "io/cmd_io_plugin.h"
#include "env/env_plugin.h"
#include "config/config_plugin.h"

using namespace rendu;

int main(int argc, char** argv)
{
    Launcher launcher;
    launcher.AddPlugin<EnvPlugin>();
    launcher.AddPlugin<ConfigPlugin>();
//    launcher.AddPlugin<CmdIoPlugin>();
    launcher.Run();
    return 0;
}