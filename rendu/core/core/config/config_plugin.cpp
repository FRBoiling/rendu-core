/*
* Created by boil on 2023/5/2.
*/

#include "config_plugin.h"
#include "config_system.h"
#include "host/host.h"

using namespace rendu;

void ConfigPlugin::Mount(Host &host) {
    host.AddSystem(std::make_shared<ConfigInitSystem>())
            .AddSystem(std::make_shared<ConfigLaterInitSystem>())
            .AddSystem(std::make_shared<ConfigUpdateSystem>());
}