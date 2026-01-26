#pragma once

#include "core/define.h"
#include <functional>
#include <atomic>

BEGIN_NAMESPACE_CORE

/**
 * @brief 主循环抽象基类
 *
 * 提供统一的循环接口，支持启动/停止/暂停/恢复
 */
class Loop {
public:
    using UpdateCallback = std::function<void(float)>;

    Loop() = default;
    virtual ~Loop() = default;

    // 禁止拷贝和移动
    Loop(const Loop&) = delete;
    Loop& operator=(const Loop&) = delete;
    Loop(Loop&&) = delete;
    Loop& operator=(Loop&&) = delete;

    /**
     * @brief 启动循环
     */
    virtual void start() = 0;

    /**
     * @brief 停止循环
     */
    virtual void stop() = 0;

    /**
     * @brief 暂停循环
     */
    virtual void pause() = 0;

    /**
     * @brief 恢复循环
     */
    virtual void resume() = 0;

    /**
     * @brief 设置更新回调
     * @param callback 更新回调函数，参数为 delta_time（秒）
     */
    void set_update_callback(UpdateCallback callback) {
        on_update_ = std::move(callback);
    }

    /**
     * @brief 检查循环是否正在运行
     */
    bool is_running() const {
        return running_.load(std::memory_order_acquire);
    }

    /**
     * @brief 检查循环是否已暂停
     */
    bool is_paused() const {
        return paused_.load(std::memory_order_acquire);
    }

protected:
    UpdateCallback on_update_;
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
};

END_NAMESPACE_CORE
