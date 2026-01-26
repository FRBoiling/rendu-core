#pragma once

#include "core/define.h"
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <common/io/io_context.h>
#include <common/log/logger.h>
#include "core/engine/context.h"

BEGIN_NAMESPACE_CORE

/**
 * @brief 引擎模块接口
 *
 * 所有可添加到引擎的模块都需要实现此接口
 */
class IEngineModule {
public:
    virtual ~IEngineModule() = default;

    /**
     * @brief 初始化模块
     */
    virtual void initialize() {}

    /**
     * @brief 每帧更新
     * @param delta_time 距离上一帧的时间（秒）
     */
    virtual void update(float delta_time) {}

    /**
     * @brief 停止模块
     */
    virtual void shutdown() {}
};

/**
 * @brief 主引擎类
 *
 * 管理引擎生命周期和所有子系统
 */
class Engine {
public:
    Engine();
    ~Engine();

    // 禁止拷贝和移动
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    // 初始化
    void initialize();

    // 启动/停止
    void start();
    void stop();

    // 运行状态
    bool is_running() const;

    // 访问上下文
    Context& context();
    const Context& context() const;

    // 模块管理
    template<typename Module, typename... Args>
    void add_module(Args&&... args) {
        auto module = std::make_shared<Module>(std::forward<Args>(args)...);
        module->initialize();
        modules_.push_back(module);
    }

    void remove_module(std::shared_ptr<IEngineModule> module);

    // 设置更新频率（可选）
    void set_target_fps(int fps);

private:
    void run_loop();
    void update_modules(float delta_time);

    std::unique_ptr<Context> context_;
    std::vector<std::shared_ptr<IEngineModule>> modules_;
    std::atomic<bool> running_;
    std::thread loop_thread_;
    std::mutex modules_mutex_;

    float target_delta_time_; // 目标帧间隔（秒）
    int target_fps_;
};

END_NAMESPACE_CORE
