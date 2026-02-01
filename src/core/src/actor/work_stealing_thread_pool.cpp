#include "core/actor/work_stealing_thread_pool.h"
#include <common/log/logger.h>

BEGIN_NAMESPACE_CORE

WorkStealingThreadPool::WorkStealingThreadPool(size_t thread_count)
    : stopped_(false)
    , thread_count_(thread_count > 0 ? thread_count : 1)
    , rng_(rd_())
{
    thread_data_.resize(thread_count_);

    // 创建工作线程和线程数据
    for (size_t i = 0; i < thread_count_; ++i) {
        thread_data_[i] = std::make_unique<ThreadData>();
        workers_.emplace_back(&WorkStealingThreadPool::worker_loop, this, i);

        // 注册线程 ID 映射
        {
            std::lock_guard<std::mutex> lock(thread_id_map_mutex_);
            thread_id_map_[workers_.back().get_id()] = i;
        }
    }

    RENDU_LOG_INFO("WorkStealingThreadPool created with {} threads", thread_count_);
}

WorkStealingThreadPool::~WorkStealingThreadPool() {
    stop();
}

void WorkStealingThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(thread_id_map_mutex_);
        if (stopped_) return;
        stopped_ = true;
    }

    // 通知所有线程
    for (auto& data_ptr : thread_data_) {
        auto& data = *data_ptr;
        {
            std::lock_guard<std::mutex> lock(data.mutex);
            data.cv.notify_all();
        }
    }

    // 等待所有线程退出
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers_.clear();
    RENDU_LOG_INFO("WorkStealingThreadPool stopped");
}

void WorkStealingThreadPool::submit(Task&& task) {
    if (!task) {
        RENDU_LOG_WARN("Attempted to submit null task");
        return;
    }

    // 获取当前线程 ID
    size_t current_thread_id = get_current_thread_id();

    // 如果当前线程不是工作线程，随机选择一个目标线程
    if (current_thread_id == thread_count_) {
        std::uniform_int_distribution<size_t> dist(0, thread_count_ - 1);
        current_thread_id = dist(rng_);
    }

    submit_to_thread(current_thread_id, std::move(task));
}

void WorkStealingThreadPool::submit_to_thread(size_t thread_id, Task&& task) {
    if (!task) {
        RENDU_LOG_WARN("Attempted to submit null task to thread {}", thread_id);
        return;
    }

    if (thread_id >= thread_count_) {
        RENDU_LOG_ERROR("Invalid thread_id: {}", thread_id);
        return;
    }

    auto& data = *thread_data_[thread_id];

    {
        std::lock_guard<std::mutex> lock(data.mutex);
        data.local_queue.push(std::move(task));
        data.submitted_count++;
        data.cv.notify_one();
    }
}

WorkStealingThreadPool::Task WorkStealingThreadPool::pop_local(size_t thread_id) {
    auto& data = *thread_data_[thread_id];

    std::unique_lock<std::mutex> lock(data.mutex);

    if (data.local_queue.empty()) {
        return nullptr;
    }

    Task task = std::move(data.local_queue.front());
    data.local_queue.pop();
    data.executed_count++;

    return task;
}

WorkStealingThreadPool::Task WorkStealingThreadPool::steal_task(size_t exclude_thread_id) {
    // 随机选择一个线程进行窃取
    if (thread_count_ <= 1) {
        return nullptr;
    }

    std::uniform_int_distribution<size_t> dist(0, thread_count_ - 2);
    size_t target_thread_id = dist(rng_);
    if (target_thread_id >= exclude_thread_id) {
        target_thread_id++;
    }

    auto& data = *thread_data_[target_thread_id];

    std::lock_guard<std::mutex> lock(data.mutex);

    if (!data.local_queue.empty()) {
        Task task = std::move(data.local_queue.front());
        data.local_queue.pop();
        // stolen_count 在这里不增加，因为任务可能由任何线程执行
        return task;
    }

    return nullptr;
}

size_t WorkStealingThreadPool::get_current_thread_id() const {
    auto thread_id = std::this_thread::get_id();

    std::lock_guard<std::mutex> lock(thread_id_map_mutex_);
    auto it = thread_id_map_.find(thread_id);
    if (it != thread_id_map_.end()) {
        return it->second;
    }

    return thread_count_; // 返回一个无效的线程 ID
}

void WorkStealingThreadPool::worker_loop(size_t thread_id) {
    RENDU_LOG_DEBUG("Worker thread {} started", thread_id);

    auto& data = *thread_data_[thread_id];

    while (true) {
        Task task = nullptr;

        // 1. 尝试从本地队列获取任务
        task = pop_local(thread_id);

        if (!task) {
            // 2. 本地队列为空，等待新任务或超时后尝试窃取
            std::unique_lock<std::mutex> lock(data.mutex);
            data.cv.wait_for(lock, std::chrono::milliseconds(10), [this, &data]() {
                return stopped_ || !data.local_queue.empty();
            });

            if (stopped_ && data.local_queue.empty()) {
                break;
            }

            // 再次检查本地队列
            if (!data.local_queue.empty()) {
                task = std::move(data.local_queue.front());
                data.local_queue.pop();
                data.executed_count++;
            } else {
                // 3. 尝试从其他线程窃取任务
                lock.unlock();
                task = steal_task(thread_id);
                if (task) {
                    // 统计窃取的任务数
                    data.stolen_count++;
                }
            }
        }

        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                RENDU_LOG_ERROR("Worker thread {} task error: {}", thread_id, e.what());
            }
        }
    }

    RENDU_LOG_DEBUG("Worker thread {} stopped", thread_id);
}

WorkStealingThreadPool::Stats WorkStealingThreadPool::get_stats() const {
    Stats stats;

    for (const auto& data_ptr : thread_data_) {
        const auto& data = *data_ptr;
        std::lock_guard<std::mutex> lock(data.mutex);
        stats.submitted_count += data.submitted_count;
        stats.executed_count += data.executed_count;
        stats.stolen_count += data.stolen_count;
        stats.idle_count += data.idle_count;
    }

    return stats;
}

END_NAMESPACE_CORE
