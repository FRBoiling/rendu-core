/*
* Created by boil on 25-4-9.
*/
#pragma once

#include "core/ecs/System.hpp"
#include "core/ecs/World.hpp"

#include <chrono>
#include <mutex>
#include <algorithm>

class TimerSystem : public core::System {
public:
    struct TimerTask {
        std::chrono::steady_clock::time_point executeTime;
        std::function<void()> callback;

        bool operator<(const TimerTask& other) const {
            return executeTime > other.executeTime; // 用于优先队列
        }
    };

    void update(double deltaTime) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();

        while (!tasks_.empty() && now >= tasks_.top().executeTime) {
            auto task = std::move(tasks_.top());
            tasks_.pop();
            task.callback();
        }
    }

    void addTimer(uint32_t delayMs, std::function<void()> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto executeTime = std::chrono::steady_clock::now() +
                         std::chrono::milliseconds(delayMs);
        tasks_.push({executeTime, std::move(callback)});
    }

private:
    std::priority_queue<TimerTask> tasks_;
    std::mutex mutex_;
};

