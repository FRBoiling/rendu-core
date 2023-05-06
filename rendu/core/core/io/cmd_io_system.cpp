/*
* Created by boil on 2023/3/19.
*/

#include "cmd_io_system.h"
#include "log/logger.h"
#include <iostream>

using namespace rendu;

CmdIoUpdateSystem::CmdIoUpdateSystem() = default;

SystemType CmdIoUpdateSystem::GetSystemType() {
    return SystemType::Update;
}

void CmdIoUpdateSystem::Run(Entity &entity) {
    RD_INFO("{} Run", typeid(GetClassType()).name());
    std::string name;
    RD_INFO("Enter the name: ");
    // Get the input from std::cin and store into name
    std::getline(std::cin, name);

    RD_INFO("the is name: {}",name);
}
