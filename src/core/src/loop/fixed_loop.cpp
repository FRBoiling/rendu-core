#include "core/loop/fixed_loop.h"
#include <common/log/logger.h>

BEGIN_NAMESPACE_CORE

FixedLoop::FixedLoop(float fixed_delta_time, io::IoContext& io)
    : fixed_delta_time_(fixed_delta_time)
    , io_(io)
    , target_fps_(static_cast<int>(std::round(1.0f / fixed_delta_time)))
    , last_time_(std::chrono::steady_clock::now()) {
    RENDU_LOG_INFO("FixedLoop created: delta_time={:.3f}s, target_fps={}", fixed_delta_time, target_fps_);
}

FixedLoop::~FixedLoop() {
    stop();
}

void FixedLoop::start() {
    if (running_.load(std::memory_order_acquire)) {
        RENDU_LOG_WARN("FixedLoop is already running");
        return;
    }

    running_.store(true, std::memory_order_release);
    should_stop_.store(false, std::memory_order_release);
    paused_.store(false, std::memory_order_release);
    should_pause_.store(false, std::memory_order_release);

    last_time_ = std::chrono::steady_clock::now();
    frame_count_ = 0;

    loop_thread_ = std::thread(&FixedLoop::run_loop, this);

    RENDU_LOG_INFO("FixedLoop started");
}

void FixedLoop::stop() {
    if (!running_.load(std::memory_order_acquire)) {
        return;
    }

    should_stop_.store(true, std::memory_order_release);

    if (loop_thread_.joinable()) {
        loop_thread_.join();
    }

    running_.store(false, std::memory_order_release);
    paused_.store(false, std::memory_order_release);

    RENDU_LOG_INFO("FixedLoop stopped: total_frames={}", total_frames_.load());
}

void FixedLoop::pause() {
    if (!running_.load(std::memory_order_acquire) || paused_.load(std::memory_order_acquire)) {
        return;
    }

    should_pause_.store(true, std::memory_order_release);
    RENDU_LOG_INFO("FixedLoop pausing...");
}

void FixedLoop::resume() {
    if (!running_.load(std::memory_order_acquire) || !paused_.load(std::memory_order_acquire)) {
        return;
    }

    should_resume_.store(true, std::memory_order_release);
    RENDU_LOG_INFO("FixedLoop resuming...");
}

void FixedLoop::set_target_fps(int fps) {
    if (fps <= 0) {
        RENDU_LOG_ERROR("Invalid target FPS: {}", fps);
        return;
    }

    target_fps_ = fps;
    fixed_delta_time_ = 1.0f / static_cast<float>(fps);

    RENDU_LOG_INFO("FixedLoop target FPS updated: {} ({:.3f}s)", fps, fixed_delta_time_);
}

void FixedLoop::run_loop() {
    auto last_frame_time = std::chrono::steady_clock::now();
    float accumulated_time = 0.0f;

    while (!should_stop_.load(std::memory_order_acquire)) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<float>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        // 处理暂停
        if (paused_.load(std::memory_order_acquire)) {
            if (should_resume_.load(std::memory_order_acquire)) {
                paused_.store(false, std::memory_order_release);
                should_resume_.store(false, std::memory_order_release);
                last_frame_time = std::chrono::steady_clock::now();
            } else {
                // 暂停状态下等待
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
        }

        if (should_pause_.load(std::memory_order_acquire)) {
            paused_.store(true, std::memory_order_release);
            should_pause_.store(false, std::memory_order_release);
            last_frame_time = std::chrono::steady_clock::now();
            continue;
        }

        // 累积时间
        accumulated_time += elapsed;

        // 固定步长更新
        while (accumulated_time >= fixed_delta_time_ && !should_stop_.load(std::memory_order_acquire)) {
            if (on_update_) {
                on_update_(fixed_delta_time_);
            }

            accumulated_time -= fixed_delta_time_;
            total_frames_.fetch_add(1, std::memory_order_release);
            frame_count_++;
        }

        // 计算 FPS
        calculate_fps();

        // 帧率控制
        if (accumulated_time < fixed_delta_time_) {
            auto sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::duration<float>(fixed_delta_time_ - accumulated_time)
            );
            if (sleep_time.count() > 0) {
                std::this_thread::sleep_for(sleep_time);
            }
        }
    }
}

void FixedLoop::calculate_fps() {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<float>(current_time - last_time_).count();

    if (elapsed >= 1.0f) {
        float fps = static_cast<float>(frame_count_) / elapsed;
        current_fps_.store(fps, std::memory_order_release);

        frame_count_ = 0;
        last_time_ = current_time;
    }
}

END_NAMESPACE_CORE
