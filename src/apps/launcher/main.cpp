#include <iostream>
#include "common/ecs/application.h"
#include "common/logging/log.h"
#include "core/logging/logger_system.h"


using namespace Rendu::Ecs;
using namespace Rendu::Core;
using namespace Rendu;

// 示例应用程序
class MyApplication : public Application
{
protected:
    void register_systems() override
    {
        world_->add_system<LoggerSystem>();
    }

    void update(float delta_time) override
    {
        // 应用程序特定更新逻辑
    }
};

int main(int argc, char* argv[])
{
    try
    {
        MyApplication app;

        if (!app.initialize())
        {
            RC_LOG_FATAL("application", "Failed to initialize application");
            return 1;
        }

        RC_LOG_INFO("application", "Starting application: {}", app.get_name());
        app.run();

        return 0;
    }
    catch (const std::exception& e)
    {
        RC_LOG_FATAL("application", "Unhandled exception:{}", e.what());
        return 1;
    }
}
