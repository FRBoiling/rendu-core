/*
* Created by boil on 2023/3/19.
*/

#include "cmd_io_system.h"

using namespace rendu;

CmdIoSystem::CmdIoSystem() = default;

void CmdIoSystem::Run(EntityPool &registry) {

}

SystemType CmdIoSystem::GetSystemType() {
    return SystemType::Update;
}
