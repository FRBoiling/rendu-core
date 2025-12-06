#include "core/logging/logger_system.h"
#include "common/logging/log.h"
#include "common/ecs/application.h"
#include <thread>
#include "common/ecs/world.h"
#include "core/async_event/async_event_system.h"

BEGIN_NAMESPACE_CORE
    LoggerSystem::~LoggerSystem()
    {
        shutdown();
        cleanup();
    }

    void LoggerSystem::configure(Ecs::World* world)
    {
        world_ = world;
        if (!world_)
        {
            RC_LOG_ERROR("application", "World is not available");
            return;
        }

        // 从World获取AsyncEventSystem
        auto* async_event_system = world_->get_system<AsyncEventSystem>();
        if (!async_event_system)
        {
            RC_LOG_ERROR("application", "AsyncEventSystem is not available");
            return;
        }

        auto& ioContext = async_event_system->get_io_context();

        auto log_instance = Logging::Log::instance();
        log_instance->CreateAppenderFromConfigLine("Appender.Default", "1,2,7,13 11 9 5 3 1"); // DEBUG级别，控制台输出
        log_instance->CreateLoggerFromConfigLine("Logger.application", "2,Default"); // DEBUG级别，使用DefaultAppender
        log_instance->Initialize(&ioContext);
    }

    void LoggerSystem::initialize()
    {
        if (initialized_)
            return;

        initialized_ = true;
        RC_LOG_DEBUG("application", "LoggerSystem initialized");
    }

    void LoggerSystem::update_sequential(float delta_time)
    {
        // 日志系统在有序阶段不需要特殊处理
        // 日志是异步的，不需要每帧更新
    }

    void LoggerSystem::update_parallel(float delta_time)
    {
        // 日志系统在并行阶段不需要特殊处理
        // 日志是异步的，不需要每帧更新
    }

    void LoggerSystem::shutdown()
    {
        if (!initialized_)
            return;

        RC_LOG_INFO("application", "LoggerSystem shutdown");
        initialized_ = false;
    }

    void LoggerSystem::cleanup()
    {
        // 日志系统没有需要特别清理的资源
    }

    std::string LoggerSystem::get_name() const
    {
        return "LoggerSystem";
    }

    std::vector<std::string> LoggerSystem::get_dependencies() const
    {
        return {"AsyncEventSystem"};  // 依赖AsyncEventSystem（需要其IoContext）
    }

    Ecs::SystemExecutionMode LoggerSystem::get_execution_mode() const
    {
        return Ecs::SystemExecutionMode::SEQUENTIAL;  // 日志系统必须串行执行
    }

    std::shared_ptr<LoggerSystem> LoggerSystem::get_logger()
    {
        static std::shared_ptr<LoggerSystem> instance = std::make_shared<LoggerSystem>();
        return instance;
    }

END_NAMESPACE_CORE
