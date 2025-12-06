//
// Created by 沸腾 on 2025/12/7.
//

#ifndef RENDU_THREAD_POOL_STATS_H
#define RENDU_THREAD_POOL_STATS_H
#include <atomic>

#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 线程池性能统计结构 - 修复原子变量的复制问题
        struct ThreadPoolStats
        {
            std::atomic<uint64_t> total_tasks_processed{0};
            std::atomic<uint64_t> tasks_stolen{0};
            std::atomic<uint64_t> tasks_executed{0};
            std::atomic<uint64_t> queue_size{0};
            std::atomic<uint64_t> max_queue_size{0};
            std::chrono::steady_clock::time_point start_time;

            ThreadPoolStats() : start_time(std::chrono::steady_clock::now())
            {
            }

            // 删除复制构造函数和赋值操作符，因为原子变量不可复制
            ThreadPoolStats(const ThreadPoolStats&) = delete;
            ThreadPoolStats& operator=(const ThreadPoolStats&) = delete;

            // 添加移动构造函数和移动赋值操作符
            ThreadPoolStats(ThreadPoolStats&& other) noexcept
                : total_tasks_processed(other.total_tasks_processed.load())
                  , tasks_stolen(other.tasks_stolen.load())
                  , tasks_executed(other.tasks_executed.load())
                  , queue_size(other.queue_size.load())
                  , max_queue_size(other.max_queue_size.load())
                  , start_time(other.start_time)
            {
            }

            ThreadPoolStats& operator=(ThreadPoolStats&& other) noexcept
            {
                if (this != &other)
                {
                    total_tasks_processed.store(other.total_tasks_processed.load());
                    tasks_stolen.store(other.tasks_stolen.load());
                    tasks_executed.store(other.tasks_executed.load());
                    queue_size.store(other.queue_size.load());
                    max_queue_size.store(other.max_queue_size.load());
                    start_time = other.start_time;
                }
                return *this;
            }

            double GetUptimeSeconds() const
            {
                auto now = std::chrono::steady_clock::now();
                return std::chrono::duration<double>(now - start_time).count();
            }

            double GetTasksPerSecond() const
            {
                auto uptime = GetUptimeSeconds();
                return uptime > 0 ? static_cast<double>(total_tasks_processed.load()) / uptime : 0.0;
            }
        };
    }

END_NAMESPACE_COMMON

#endif //RENDU_THREAD_POOL_STATS_H
