#include "common/ecs/logger_system.h"
#include <iostream>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        LoggerSystem::LoggerSystem()
        {
        }
        
        void LoggerSystem::configure(World* world)
        {
            world_ = world;
        }
        
        void LoggerSystem::initialize()
        {
            // 初始化日志系统
            if (!initialized_ && Logger::get_instance().initialize(application_name_))
            {
                initialized_ = true;
                LOG_INFO("LoggerSystem initialized successfully");
            }
            else if (!initialized_)
            {
                std::cerr << "Failed to initialize LoggerSystem!" << std::endl;
            }
        }
        
        void LoggerSystem::update(float delta_time)
        {
            // 日志系统通常不需要在每一帧更新
            // 但可以在这里添加周期性的日志维护任务（如日志滚动）
        }
        
        void LoggerSystem::shutdown()
        {
            if (initialized_)
            {
                LOG_INFO("LoggerSystem shutting down");
                Logger::get_instance().shutdown();
                initialized_ = false;
            }
        }
        
        std::string LoggerSystem::get_name() const
        {
            return "LoggerSystem";
        }
        
        std::vector<std::string> LoggerSystem::get_dependencies() const
        {
            // 日志系统通常是基础系统，没有依赖
            return {};
        }
        
        Logger& LoggerSystem::get_logger()
        {
            return Logger::get_instance();
        }
    }
END_NAMESPACE_COMMON