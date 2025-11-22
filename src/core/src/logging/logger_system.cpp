#include "core/logging/logger_system.h"
#include "common/logging/log.h"
#include "common/asio/io_context.h"
#include <iostream>

#include "common/logging/appender_default.h"

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
                // 获取Log单例实例
                auto* log_instance = Logging::Log::instance();
                log_instance->RegisterAppender<Logging::AppenderDefault>();

                // 创建一个IoContext用于日志系统
                static Asio::IoContext io_context;
                // 初始化日志系统
                log_instance->Initialize(&io_context);
                
                // 手动创建默认配置（因为LoadFromConfig中的配置读取被注释掉了）
                // 创建默认Appender
                log_instance->CreateAppenderFromConfigLine("DefaultAppender", "2,0,1"); // INFO级别，控制台输出
                
                // 创建默认Logger
                log_instance->CreateLoggerFromConfigLine("root", "2,DefaultAppender"); // INFO级别，使用DefaultAppender
                log_instance->CreateLoggerFromConfigLine("application", "2,DefaultAppender"); // INFO级别，使用DefaultAppender

                initialized_ = true;
                std::cout << "LoggerSystem initialized successfully with logging system" << std::endl;
                
                // 现在可以使用RC_LOG_INFO宏记录日志了
                RC_LOG_INFO("application", "LoggerSystem initialized successfully{}");
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