#include "common/ecs/application.h"
#include "common/logging/log.h"
#include "common/banner.h"
#include <chrono>
#include <iostream>
#include <sys/syslog.h>

#include "common/ecs/world.h"
#include "common/logging/appender_default.h"

using namespace Rendu;
using namespace Rendu::Ecs;
using namespace Rendu::Logging;

Application::Application()
{
    world_ = std::make_unique<World>(this);
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize()
{
    try
    {
        auto log_instance =Log::instance();
        log_instance->CreateAppenderFromConfigLine("Appender.Default", "1,2,7,13 11 9 5 3 1"); // Trace级别，控制台输出
        log_instance->CreateLoggerFromConfigLine("Logger.application", "2,Default"); // Trace级别，使用DefaultAppender
        log_instance->Initialize(nullptr);

        register_systems();
        world_->configure_systems();
        world_->initialize_systems();

        // 现在使用我们的日志系统显示banner
        Banner::Show(
            "RenduCore", // 应用名称
            [](char const* text)
            {
                RC_LOG_INFO("application", "{}", text);
            }, // 使用日志系统的INFO级别
            [](char const* text)
            {
                RC_LOG_DEBUG("application", "{}", text);
            } // 使用日志系统的DEBUG级别
        );
        // 将日志记录移到系统初始化完成之后
        RC_LOG_INFO("application", "{}", "Application initialized successfully");

        running_ = true;
        return true;
    }
    catch (const std::exception& e)
    {
        // 如果日志系统初始化失败，使用std::cerr作为备用
        std::cerr << "Failed to initialize application: " << e.what() << std::endl;
        RC_LOG_ERROR("application", "Failed to initialize application: {}", e.what());
        return false;
    }
}

void Application::run()
{
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();

    while (running_)
    {
        // 计算delta time
        auto current_time = clock::now();
        auto delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;

        // 应用程序特定更新
        update(delta_time);

        // 使用帧率限制器
        frame_rate_limiter_.limit();
    }

}

void Application::update(float delta_time)
{
    world_->update_systems(delta_time);
}

void Application::shutdown()
{
    if (running_)
    {
        running_ = false;

        // 关闭系统
        world_->shutdown_systems();

        world_.reset();
    }
}

void Application::set_target_fps(float fps)
{
    frame_rate_limiter_.set_target_fps(fps);
}

float Application::get_target_fps() const
{
    return frame_rate_limiter_.get_target_fps();
}

float Application::get_actual_fps() const
{
    return frame_rate_limiter_.get_actual_fps();
}
