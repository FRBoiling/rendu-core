#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <future>
#include <random>
#include <memory>
#include <unordered_map>

#include "core/define.h"

BEGIN_NAMESPACE_CORE

/**
 * @brief 工作窃取线程池
 *
 * 优化策略：
 * - 每个线程拥有独立的任务队列（减少锁竞争）
 * - 线程空闲时从其他线程队列窃取任务（负载均衡）
 * - 随机选择目标线程窃取（避免热点）
 *
 * 预期收益：
 * - 减少 80%+ 线程创建/销毁
 * - 内存占用降低 40%+
 * - 支持 1000+ Actor
 */
class WorkStealingThreadPool {
public:
    /**
     * @brief 任务类型
     */
    using Task = std::function<void()>;

    /**
     * @brief 构造函数
     * @param thread_count 线程数量，默认使用硬件并发数
     */
    explicit WorkStealingThreadPool(size_t thread_count = std::thread::hardware_concurrency());

    /**
     * @brief 析构函数
     */
    ~WorkStealingThreadPool();

    /**
     * @brief 提交任务到本地线程队列
     * @param task 任务
     */
    void submit(Task&& task);

    /**
     * @brief 提交任务到指定线程队列
     * @param thread_id 目标线程 ID
     * @param task 任务
     */
    void submit_to_thread(size_t thread_id, Task&& task);

    /**
     * @brief 停止线程池
     */
    void stop();

    /**
     * @brief 获取线程数量
     */
    size_t thread_count() const { return thread_count_; }

    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t submitted_count = 0;      // 提交的任务数
        size_t executed_count = 0;       // 执行的任务数
        size_t stolen_count = 0;        // 窃取的任务数
        size_t idle_count = 0;         // 空闲次数
    };

    Stats get_stats() const;

private:
    /**
     * @brief 工作线程主循环
     * @param thread_id 线程 ID
     */
    void worker_loop(size_t thread_id);

    /**
     * @brief 从本地队列获取任务
     * @param thread_id 线程 ID
     * @return 任务（nullptr 表示没有任务）
     */
    Task pop_local(size_t thread_id);

    /**
     * @brief 从其他线程窃取任务
     * @param exclude_thread_id 排除的线程 ID
     * @return 任务（nullptr 表示没有任务）
     */
    Task steal_task(size_t exclude_thread_id);

    /**
     * @brief 获取当前线程 ID
     */
    size_t get_current_thread_id() const;

private:
    /**
     * @brief 线程本地数据
     */
    struct ThreadData {
        mutable std::queue<Task> local_queue;
        mutable std::mutex mutex;
        std::condition_variable cv;
        size_t submitted_count = 0;
        size_t executed_count = 0;
        size_t stolen_count = 0;
        size_t idle_count = 0;

        ThreadData() = default;
        ThreadData(const ThreadData&) = delete;
        ThreadData& operator=(const ThreadData&) = delete;
        ThreadData(ThreadData&&) = delete;
        ThreadData& operator=(ThreadData&&) = delete;
    };

    std::vector<std::thread> workers_;
    std::vector<std::unique_ptr<ThreadData>> thread_data_;
    std::atomic<bool> stopped_;
    size_t thread_count_;

    // 线程 ID 映射
    std::unordered_map<std::thread::id, size_t> thread_id_map_;
    mutable std::mutex thread_id_map_mutex_;

    // 随机数生成器（用于工作窃取）
    mutable std::random_device rd_;
    mutable std::mt19937 rng_;
};

END_NAMESPACE_CORE
