#pragma once

#include "core/loop/loop.h"
#include <chrono>
#include <thread>
#include <common/io/io_context.h>
#include <atomic>

BEGIN_NAMESPACE_CORE

/**
 * @brief 固定步长主循环
 *
 * 使用固定的时间步长更新，适合物理模拟等需要确定性的场景
 */
class FixedLoop : public Loop {
public:
    /**
     * @brief 构造函数
     * @param fixed_delta_time 固定时间步长（秒）
     * @param io IO 上下文引用
     */
    explicit FixedLoop(float fixed_delta_time, io::IoContext& io);
    ~FixedLoop() override;

    // 禁止拷贝和移动
    FixedLoop(const FixedLoop&) = delete;
    FixedLoop& operator=(const FixedLoop&) = delete;
    FixedLoop(FixedLoop&&) = delete;
    FixedLoop& operator=(FixedLoop&&) = delete;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;

    /**
     * @brief 设置目标帧率
     * @param fps 目标帧率
     */
    void set_target_fps(int fps);

    /**
     * @brief 获取目标帧率
     */
    int target_fps() const {
        return target_fps_;
    }

    /**
     * @brief 获取固定时间步长
     */
    float fixed_delta_time() const {
        return fixed_delta_time_;
    }

    /**
     * @brief 获取当前帧率
     */
    float current_fps() const {
        return current_fps_.load(std::memory_order_acquire);
    }

    /**
     * @brief 获取总帧数
     */
    uint64_t total_frames() const {
        return total_frames_.load(std::memory_order_acquire);
    }

private:
    void run_loop();
    void calculate_fps();

    float fixed_delta_time_;
    io::IoContext& io_;
    std::thread loop_thread_;

    int target_fps_;
    std::atomic<bool> should_stop_{false};
    std::atomic<bool> should_pause_{false};
    std::atomic<bool> should_resume_{false};

    // FPS 统计
    std::atomic<float> current_fps_{0.0f};
    std::atomic<uint64_t> total_frames_{0};
    std::chrono::steady_clock::time_point last_time_;
    uint64_t frame_count_{0};
};

END_NAMESPACE_CORE
