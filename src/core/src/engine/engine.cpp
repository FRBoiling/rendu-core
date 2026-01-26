//**********************************
//  Created by boil on 2026/01/26.
//**********************************

#include "core/engine/engine.h"
#include <chrono>
#include <thread>

BEGIN_NAMESPACE_CORE

Engine::Engine()
    : context_(std::make_unique<Context>())
    , modules_()
    , running_(false)
    , loop_thread_()
    , modules_mutex_()
    , target_delta_time_(0.016f) // 默认 60 FPS
    , target_fps_(60) {
}

Engine::~Engine() {
    if (running_) {
        stop();
    }
}

void Engine::initialize() {
    context_->logger().info("引擎初始化中...");
    context_->logger().info("目标帧率: " + std::to_string(target_fps_) + " FPS");
    context_->logger().info("引擎初始化完成");
}

void Engine::start() {
    if (running_) {
        context_->logger().warn("引擎已在运行中");
        return;
    }

    running_ = true;
    loop_thread_ = std::thread(&Engine::run_loop, this);
    context_->logger().info("引擎已启动");
}

void Engine::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    if (loop_thread_.joinable()) {
        loop_thread_.join();
    }

    // 清理模块
    {
        std::lock_guard<std::mutex> lock(modules_mutex_);
        for (auto& module : modules_) {
            module->shutdown();
        }
    }

    context_->logger().info("引擎已停止");
}

bool Engine::is_running() const {
    return running_;
}

Context& Engine::context() {
    return *context_;
}

const Context& Engine::context() const {
    return *context_;
}

void Engine::remove_module(std::shared_ptr<IEngineModule> module) {
    std::lock_guard<std::mutex> lock(modules_mutex_);
    module->shutdown();
    modules_.erase(
        std::remove(modules_.begin(), modules_.end(), module),
        modules_.end());
}

void Engine::set_target_fps(int fps) {
    if (fps <= 0) {
        context_->logger().warn("无效的目标帧率: " + std::to_string(fps) + "，使用默认值 60");
        target_fps_ = 60;
        target_delta_time_ = 0.016f;
        return;
    }

    target_fps_ = fps;
    target_delta_time_ = 1.0f / fps;
    context_->logger().info("目标帧率已设置为: " + std::to_string(fps) + " FPS");
}

void Engine::run_loop() {
    using clock = std::chrono::high_resolution_clock;
    using namespace std::chrono;

    auto last_time = clock::now();

    while (running_) {
        auto current_time = clock::now();
        auto elapsed = duration_cast<microseconds>(current_time - last_time).count();
        float delta_time = elapsed / 1000000.0f; // 转换为秒

        // 更新所有模块
        update_modules(delta_time);

        // 控制帧率
        auto frame_time = duration_cast<microseconds>(clock::now() - current_time).count();
        auto sleep_time = static_cast<long long>(target_delta_time_ * 1000000) - frame_time;

        if (sleep_time > 0) {
            std::this_thread::sleep_for(microseconds(sleep_time));
        }

        last_time = current_time;
    }
}

void Engine::update_modules(float delta_time) {
    std::lock_guard<std::mutex> lock(modules_mutex_);
    for (auto& module : modules_) {
        module->update(delta_time);
    }
}

END_NAMESPACE_CORE
