#include "common/ecs/application.h"
#include <chrono>
#include <iostream>

#include "common/banner.h"
#include "common/ecs/logger_system.h"  // 添加LoggerSystem头文件

using namespace common;
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
        // 初始化并注册LoggerSystem
        auto* logger_system = world_->add_system<LoggerSystem>();
        if (!logger_system)
        {
            std::cerr << "Failed to create LoggerSystem!" << std::endl;
            return false;
        }
        
        // 配置系统
        world_->configure_systems();
        
        // 初始化系统
        world_->initialize_systems();
        
        // 现在使用我们的日志系统显示banner
        Banner::Show(
            "RenduCore",                    // 应用名称
            [](char const* text) { LOG_INFO(text); }, // 使用日志系统的INFO级别
            [](char const* text) { LOG_DEBUG(text); }  // 使用无参数lambda函数
        );

        // 注册其他系统（通过子类的register_systems方法）
        register_systems();
        
        // 重新配置和初始化新增的系统
        world_->configure_systems();
        world_->initialize_systems();
        
        running_ = true;
        return true;
    }
    catch (const std::exception& e)
    {
        // 使用日志系统记录错误
        LOG_ERROR("Failed to initialize application: {}", e.what());
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
        
        // 使用帧率限制器
        frame_rate_limiter_.limit();
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

// 添加新方法实现
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