#include "core/signal/signal_system.h"

#include <iostream>

#include "common/ecs/application.h"
#include <thread>
#include <vector>
#include <mutex>

#include "common/ecs/world.h"
#include "common/logging/log.h"
#include "core/async_event/async_event_system.h"

BEGIN_NAMESPACE_CORE
    SignalSystem::~SignalSystem()
    {
        shutdown();
        cleanup();
    }

    void SignalSystem::configure(Ecs::World* world)
    {
        world_ = world;
    }

    void SignalSystem::initialize()
    {
        if (initialized_)
            return;

        if (!world_ || !world_->get_application())
        {
            std::cerr << "SignalSystem: World or Application is not available" << std::endl;
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

        // 按照你提供的模式完善信号设置
        // 创建信号处理器，初始添加SIGINT和SIGTERM信号
        signals_ = std::make_unique<Asio::SignalSet>(ioContext, SIGINT, SIGTERM);

        // 在Windows平台上添加SIGBREAK信号
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
        signals_->add(SIGBREAK);
#endif

        // 开始异步等待信号
        signals_->async_wait([this](const std::error_code& error, int signal)
        {
            handle_signal(error, signal);
        });

        // 注册默认信号处理器
        register_default_signals();

        initialized_ = true;
        RC_LOG_DEBUG("application", "SignalSystem initialized");
    }

    void SignalSystem::update_sequential(float delta_time)
    {
        // 信号系统在有序阶段不需要特殊处理
        // 信号处理是异步的，通过回调函数处理
    }

    void SignalSystem::update_parallel(float delta_time)
    {
        // 信号系统在并行阶段不需要特殊处理
        // 信号处理是异步的，通过回调函数处理
    }

    void SignalSystem::shutdown()
    {
        if (!initialized_)
            return;

        RC_LOG_INFO("application", "SignalSystem shutdown");

        // 停止信号处理
        if (signals_)
        {
            signals_->cancel();
            signals_.reset();
        }

        initialized_ = false;
    }

    void SignalSystem::cleanup()
    {
        // 清空信号处理器
        {
            std::lock_guard<std::mutex> lock(handlers_mutex_);
            signal_handlers_.clear();
        }
    }

    std::string SignalSystem::get_name() const
    {
        return "SignalSystem";
    }

    std::vector<std::string> SignalSystem::get_dependencies() const
    {
        return {"AsyncEventSystem"};  // 依赖AsyncEventSystem（需要其IoContext）
    }

    Ecs::SystemExecutionMode SignalSystem::get_execution_mode() const
    {
        return Ecs::SystemExecutionMode::PARALLEL;  // 支持并行执行
    }

    void SignalSystem::register_signal_handler(int signal, SignalCallback callback)
    {
        std::lock_guard<std::mutex> lock(handlers_mutex_);
        signal_handlers_[signal].push_back(callback);

        // 如果系统已经初始化，添加信号到SignalSet
        if (initialized_ && signals_)
        {
            try
            {
                signals_->add(signal);
                std::cout << "SignalSystem: Registered handler for signal: " << signal << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cerr << "SignalSystem: Failed to add signal " << signal << ": " << e.what() << std::endl;
            }
        }
    }

    void SignalSystem::unregister_signal_handler(int signal)
    {
        std::lock_guard<std::mutex> lock(handlers_mutex_);
        signal_handlers_.erase(signal);

        if (initialized_ && signals_)
        {
            try
            {
                signals_->remove(signal);
                std::cout << "SignalSystem: Unregistered handler for signal: " << signal << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cerr << "SignalSystem: Failed to remove signal " << signal << ": " << e.what() << std::endl;
            }
        }
    }

    void SignalSystem::handle_signal(const std::error_code& error, int signal_number)
    {
        if (error)
        {
            if (error != asio::error::operation_aborted)
            {
                std::cerr << "SignalSystem: Signal error: " << error.message() << std::endl;
            }
            return;
        }

        std::cout << "SignalSystem: Received signal: " << signal_number << std::endl;

        // 执行注册的回调函数
        std::vector<SignalCallback> handlers;
        {
            std::lock_guard<std::mutex> lock(handlers_mutex_);
            auto it = signal_handlers_.find(signal_number);
            if (it != signal_handlers_.end())
            {
                handlers = it->second;
            }
        }

        for (const auto& handler : handlers)
        {
            try
            {
                handler(signal_number);
            }
            catch (const std::exception& e)
            {
                std::cerr << "SignalSystem: Signal handler error for signal " << signal_number << ": " << e.what() <<
                    std::endl;
            }
        }

        // 继续等待下一个信号
        if (initialized_ && signals_)
        {
            signals_->async_wait([this](const std::error_code& error, int signal)
            {
                handle_signal(error, signal);
            });
        }
    }

    void SignalSystem::register_default_signals()
    {
        // 注册SIGINT和SIGTERM信号的默认处理器
        register_signal_handler(SIGINT, [](int signal)
        {
            std::cout << "SignalSystem: SIGINT received, application will shutdown" << std::endl;
            // 这里可以添加应用程序关闭逻辑
        });

        register_signal_handler(SIGTERM, [](int signal)
        {
            std::cout << "SignalSystem: SIGTERM received, application will shutdown" << std::endl;
            // 这里可以添加应用程序关闭逻辑
        });

#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
        register_signal_handler(SIGBREAK, [](int signal)
        {
            std::cout << "SignalSystem: SIGBREAK received, application will shutdown" << std::endl;
            // Windows特定的关闭逻辑
        });
#endif
    }

END_NAMESPACE_CORE
