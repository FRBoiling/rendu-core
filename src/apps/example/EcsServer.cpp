//
// Created by FRee2 on 2025/10/18.
//
#include "EcsServer.h"

#include "logging/LoggingSystem.h"
#include "network/NetworkSystem.h"
#include "player/LogicSystem.h"

#include <csignal>
#include <iostream>
#include <thread>

std::unique_ptr<EcsServer> g_server;

void signal_handler(int signal) {
    std::cout << "Received signal: " << signal << std::endl;
    if (g_server) {
        g_server->stop();
    }
}

EcsServer::EcsServer()
    : work_guard_(std::make_unique<asio::io_context::work>(io_context_)) {

    // 注册信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
}

EcsServer::~EcsServer() {
    stop();
}

bool EcsServer::initialize(uint16_t port) {
    try {
        // 设置核心组件
        setup_core_components();

        // 添加核心系统
        setup_core_systems(port);

        // 初始化所有系统
        for (auto& system : systems_) {
            system->configure(registry_);
            system->initialize(registry_);
        }

        // 初始化日志系统并记录启动信息
        if (auto logging_system = get_system<LoggingSystem>()) {
            // 通过日志系统记录启动信息
            logging_system->log(registry_, spdlog::level::info,
                               fmt::format("ECS server initialized successfully on port {}", port));
        } else {
            std::cout << "ECS server initialized on port " << port << std::endl;
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize ECS server: " << e.what() << std::endl;
        return false;
    }
}

void EcsServer::run() {
    if (!running_) {
        running_ = true;

        // 启动工作线程
        unsigned int num_threads = std::thread::hardware_concurrency();
        num_threads = num_threads > 0 ? num_threads : 4; // 至少4个线程

        for (unsigned int i = 0; i < num_threads; ++i) {
            worker_threads_.emplace_back([this]() {
                try {
                    io_context_.run();
                } catch (const std::exception& e) {
                    std::cerr << "Worker thread error: " << e.what() << std::endl;
                }
            });
        }

        if (auto logging_system = get_system<LoggingSystem>()) {
            logging_system->log(registry_, spdlog::level::info,
                               fmt::format("Started {} worker threads", num_threads));
        }

        std::cout << "ECS server started with " << num_threads << " worker threads" << std::endl;

        // 运行主循环
        main_loop();
    }
}

void EcsServer::stop() {
    if (!running_) return;

    running_ = false;

    if (auto logging_system = get_system<LoggingSystem>()) {
        logging_system->log(registry_, spdlog::level::info, "Shutting down ECS server...");
    }

    // 按逆序关闭所有系统（最后创建的系统最先关闭）
    for (auto it = systems_.rbegin(); it != systems_.rend(); ++it) {
        (*it)->shutdown(registry_);
    }

    // 停止IO上下文
    work_guard_.reset();
    io_context_.stop();

    // 等待工作线程结束
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    worker_threads_.clear();

    if (auto logging_system = get_system<LoggingSystem>()) {
        logging_system->log(registry_, spdlog::level::info, "ECS server stopped successfully");
    } else {
        std::cout << "ECS server stopped" << std::endl;
    }
}

void EcsServer::setup_core_components() {
    // 注册核心组件类型（可选，主要是为了调试信息）
    registry_.type<TagComponent>();
    registry_.type<TransformComponent>();
    registry_.type<NetworkSessionComponent>();
    registry_.type<NetworkMessageComponent>();
    registry_.type<PlayerComponent>();
    registry_.type<MovementComponent>();
    registry_.type<HealthComponent>();

    if (auto logging_system = get_system<LoggingSystem>()) {
        logging_system->log(registry_, spdlog::level::debug, "Core components registered");
    }
}

void EcsServer::setup_core_systems(uint16_t port) {
    // 注意：添加顺序很重要，决定了初始化和关闭的顺序

    // 1. 首先添加日志系统（其他系统依赖它）
    add_system<LoggingSystem>("info", "logs/ecs_server.log");

    // 2. 添加网络系统
    add_system<NetworkSystem>(io_context_, port);

    // 3. 添加逻辑系统（依赖网络系统）
    add_system<LogicSystem>();

    if (auto logging_system = get_system<LoggingSystem>()) {
        logging_system->log(registry_, spdlog::level::debug,
                           fmt::format("Core systems setup completed, listening on port {}", port));
    }
}

void EcsServer::main_loop() {
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();
    uint64_t frame_count = 0;

    if (auto logging_system = get_system<LoggingSystem>()) {
        logging_system->log(registry_, spdlog::level::info, "Main loop started");
    }

    while (running_) {
        auto current_time = clock::now();
        auto delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;

        frame_count++;

        try {
            // 更新所有系统
            for (auto& system : systems_) {
                system->update(registry_, delta_time);
            }

            // 简单的帧率控制（最多60FPS）
            auto frame_time = std::chrono::duration<float>(clock::now() - current_time).count();
            float target_frame_time = 1.0f / 60.0f; // 60 FPS

            if (frame_time < target_frame_time) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(static_cast<int>((target_frame_time - frame_time) * 1000))
                );
            }

            // 每1000帧输出一次性能信息
            if (frame_count % 1000 == 0) {
                if (auto logging_system = get_system<LoggingSystem>()) {
                    auto entities_count = registry_.alive();
                    auto fps = 1.0f / delta_time;

                    logging_system->log(registry_, spdlog::level::debug,
                                       fmt::format("Frame: {}, Entities: {}, FPS: {:.1f}",
                                                  frame_count, entities_count, fps));
                }
            }

        } catch (const std::exception& e) {
            if (auto logging_system = get_system<LoggingSystem>()) {
                logging_system->log(registry_, spdlog::level::error,
                                   fmt::format("Main loop error: {}", e.what()));
            } else {
                std::cerr << "Main loop error: " << e.what() << std::endl;
            }

            // 严重错误时停止服务器
            if (std::string(e.what()).find("fatal") != std::string::npos) {
                stop();
                break;
            }
        }
    }

    if (auto logging_system = get_system<LoggingSystem>()) {
        logging_system->log(registry_, spdlog::level::info,
                           fmt::format("Main loop finished after {} frames", frame_count));
    }
}
