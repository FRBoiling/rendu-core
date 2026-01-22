#pragma once

#include "common/define.h"
#include <functional>
#include <memory>
#include <chrono>
#include <mutex>
#include <vector>
#include <common/io/io_context.h>
#include <common/io/timer.h>

BEGIN_NAMESPACE_COMMON
namespace io {

/**
 * @brief 任务调度器
 *
 * 提供任务调度功能：立即执行、延迟执行、周期性执行
 */
class Scheduler {
public:
    /**
     * @brief 取消令牌类型
     */
    using CancelToken = std::shared_ptr<std::atomic<bool>>;

    /**
     * @brief 构造函数
     * @param io I/O 上下文
     */
    explicit Scheduler(IoContext& io);
    ~Scheduler() = default;

    // 禁止拷贝
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = default;
    Scheduler& operator=(Scheduler&&) = default;

    /**
     * @brief 提交立即执行的任务
     * @param task 要执行的任务
     */
    void post(std::function<void()> task);

    /**
     * @brief 延迟执行任务
     * @param task 要执行的任务
     * @param delay 延迟时间
     * @return 取消令牌
     */
    CancelToken delayed(std::function<void()> task, std::chrono::milliseconds delay);

    /**
     * @brief 周期性执行任务
     * @param task 要执行的任务
     * @param interval 执行间隔
     * @return 取消令牌
     */
    CancelToken periodic(std::function<void()> task, std::chrono::milliseconds interval);

    /**
     * @brief 取消任务
     * @param token 取消令牌
     */
    void cancel(CancelToken token);

private:
    struct TimerTask {
        std::unique_ptr<Timer> timer;
        CancelToken token;
    };

    IoContext& io_;
    std::vector<TimerTask> timers_;
    mutable std::mutex mutex_;
};

} // namespace io
END_NAMESPACE_COMMON
