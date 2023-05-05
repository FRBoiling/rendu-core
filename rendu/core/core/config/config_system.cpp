/*
* Created by boil on 2023/5/5.
*/

#include "adapter/fwd/adapter_fwd.h"
#include "config_system.h"

using namespace rendu;

SystemType ConfigInitSystem::GetSystemType() {
    return SystemType::Init;
}

void ConfigInitSystem::Run(EntityPool &registry) {
    RD_INFO("ConfigInitSystem Run!");
}

SystemType ConfigLaterInitSystem::GetSystemType() {
    return SystemType::LateInit;
}

void ConfigLaterInitSystem::Run(EntityPool &registry) {
    RD_INFO("ConfigLaterInitSystem Run!");
}

SystemType ConfigUpdateSystem::GetSystemType() {
    return SystemType::Update;
}

void ConfigUpdateSystem::Run(EntityPool &registry) {
    RD_INFO("ConfigUpdateSystem Run!");
}


