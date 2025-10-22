#include "common/ecs/application.h"
#include <chrono>
#include <iostream>

using namespace common::Ecs;

Application::Application()
{
    // 创建世界，默认使用4个线程
    world_ = std::make_unique<World>(4);
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize()
{
    try
    {
        // 注册系统
        register_systems();

        // 配置系统
        world_->configure_systems();

        // 初始化系统
        world_->initialize_systems();

        running_ = true;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize application: " << e.what() << std::endl;
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

        // 更新系统
        world_->update_systems(delta_time);

        // 应用程序特定更新
        update(delta_time);

        // 渲染
        render();

        // 简单的帧率限制
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 约60fps
    }
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
