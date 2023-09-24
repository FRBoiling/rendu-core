/*
* Created by boil on 2023/9/6.
*/

#include "app.h"
#include "world/world.h"
#include "world/env/console_argument_parser_system.h"
#include "world/logger/logger_system.h"
#include "world/timer/time_system.h"
#include "world/fiber/fiber_manager_system.h"

RD_NAMESPACE_BEGIN

    void App::Start(int argc, char **argv) {
      World::Instance().AddSingleton<ConsoleArgumentParserSystem>(argc, argv);
      World::Instance().AddSingleton<LoggerSystem>();
      World::Instance().AddSingleton<TimeSystem>();
      World::Instance().AddSingleton<FiberManagerSystem>();
    }

    void App::Update() {
      TimeSystem::Instance().Update();
      FiberManagerSystem::Instance().Update();
    }

    void App::LateUpdate() {
      FiberManagerSystem::Instance().LateUpdate();
    }

RD_NAMESPACE_END
