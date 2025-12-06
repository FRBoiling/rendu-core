#include "common/ecs/application.h"
#include "common/ecs/world.h"
#include "common/logging/log.h"
#include <iostream>

using namespace Rendu::Ecs;
using namespace Rendu;

// 简单的应用程序示例
class ExampleApplication : public Application
{
protected:
    void register_systems() override
    {
        // 这里可以添加系统
        // world_->add_system<SomeSystem>();
    }

    void update(float delta_time) override
    {
        // 应用程序特定更新逻辑
        RC_LOG_INFO("application", "Update delta_time: {:.3f}", delta_time);
    }
};

int main()
{
    try
    {
        ExampleApplication app;

        if (!app.initialize())
        {
            RC_LOG_FATAL("application", "Failed to initialize application");
            return 1;
        }

        RC_LOG_INFO("application", "Starting example application");

        // 运行应用
        app.run();

        return 0;
    }
    catch (const std::exception& e)
    {
        RC_LOG_FATAL("application", "Unhandled exception: {}", e.what());
        return 1;
    }
}