#include "core/logging/logger_system.h"
#include "common/logging/log.h"
#include "common/asio/io_context.h"
#include "common/logging/appender_default.h"
#include <iostream>

BEGIN_NAMESPACE_CORE
    LoggerSystem::LoggerSystem()
    {
    }
    
    void LoggerSystem::configure(Ecs::World* world)
    {
        world_ = world;
    }
    
    void LoggerSystem::initialize()
    {            
        if (!initialized_)
        {                
            try
            {                    
                auto* log_instance = Logging::Log::instance();
                // 创建一个IoContext用于日志系统
                static Asio::IoContext io_context;
                log_instance->RegisterAppender<Logging::AppenderDefault>();
                log_instance->Initialize(&io_context);
                log_instance->CreateAppenderFromConfigLine("Appender.Default", "1,1,7,13 11 9 5 3 1"); // Trace级别，控制台输出
                log_instance->CreateLoggerFromConfigLine("Logger.application", "1,Default"); // Trace级别，使用DefaultAppender

                initialized_ = true;
                RC_LOG_INFO("application", "LoggerSystem initialized successfully");
            }
            catch (const std::exception& e)
            {                
                std::cerr << "Failed to initialize LoggerSystem: " << e.what() << std::endl;
            }
        }
        else
        {                
            std::cout << "LoggerSystem already initialized" << std::endl;
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
            std::cout << "LoggerSystem shutting down" << std::endl;
            
            // 关闭日志系统
            auto* log_instance = Logging::Log::instance();
            if (log_instance)
            {                
                log_instance->Close();
            }
            
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
    
    Logging::Logger& LoggerSystem::get_logger()
    {            
        // 获取Log单例中的某个Logger实例
        // 这里需要根据实际需求调整，可能需要返回一个特定类型的logger
        // 由于没有看到LoggerSystem::get_logger()的具体使用场景，这里返回一个空的logger引用
        // 实际使用时应该通过Log::instance()->GetEnabledLogger()获取正确的logger
        static Logging::Logger dummy_logger("dummy", Logging::LogLevel::LOG_LEVEL_INFO);
        return dummy_logger;
    }
END_NAMESPACE_CORE  // 修复：改为使用CORE命名空间