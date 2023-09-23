/*
* Created by boil on 2023/5/2.
*/

#include "env_plugin.h"
#include "host/host.h"
#include "env_options_system.h"

using namespace rendu;

void EnvPlugin::Mount(Host &host) {
    host.AddSystem(std::make_shared<EnvOptionsInitSystem>())
            .AddSystem(std::make_shared<EnvOptionsUpdateSystem>());
}
