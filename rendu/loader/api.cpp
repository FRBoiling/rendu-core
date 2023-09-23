/*
* Created by boil on 2023/9/6.
*/

#include "api.h"
#include "world/world.h"
#include "world/env/console_argument_parser_system.h"
#include "world/logger/logger_system.h"
#include "world/timer/time_system.h"
#include "world/fiber/fiber_manager_system.h"

RD_NAMESPACE_BEGIN

    void Api::Start(int argc, char **argv) {
      World::Instance().AddSingleton<ConsoleArgumentParserSystem>(argc, argv);
      World::Instance().AddSingleton<LoggerSystem>();
      World::Instance().AddSingleton<TimeSystem>();
      World::Instance().AddSingleton<FiberManagerSystem>();
    }

    void Api::Update() {
      TimeSystem::Instance().Update();
      FiberManagerSystem::Instance().Update();
    }

    void Api::LateUpdate() {
      FiberManagerSystem::Instance().LateUpdate();
    }

RD_NAMESPACE_END
