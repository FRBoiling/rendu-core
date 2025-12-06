#pragma once

#include "common/define.h"
#include "common/threading/thread_pool_adapter.h"
#include <functional>
#include <future>
#include <vector>
#include <cstddef>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 并行执行工具类 - 支持任务分片并行化
        class ParallelFor
        {
        public:
            // 并行遍历范围 [begin, end)
            // chunk_size: 每个任务处理的元素数量，默认256
            static void range(std::size_t begin, std::size_t end,
                            std::function<void(std::size_t, std::size_t)> func,
                            Threading::ThreadPoolAdapter& thread_pool,
                            std::size_t chunk_size = 256);

            // 并行遍历容器（通过索引）
            template <typename Container, typename Func>
            static void each(Container& container, Func&& func,
                            Threading::ThreadPoolAdapter& thread_pool,
                            std::size_t chunk_size = 256)
            {
                const std::size_t size = container.size();
                if (size == 0) return;

                range(0, size,
                    [&container, &func](std::size_t begin, std::size_t end) {
                        for (std::size_t i = begin; i < end; ++i)
                        {
                            func(container[i], i);
                        }
                    },
                    thread_pool,
                    chunk_size
                );
            }

            // 并行执行一组独立任务
            template <typename TaskType>
            static void tasks(const std::vector<TaskType>& tasks,
                            Threading::ThreadPoolAdapter& thread_pool)
            {
                if (tasks.empty()) return;

                std::vector<std::future<void>> futures;
                futures.reserve(tasks.size());

                for (const auto& task : tasks)
                {
                    futures.push_back(thread_pool.Submit(task));
                }

                for (auto& future : futures)
                {
                    future.wait();
                }
            }
        };

        inline void ParallelFor::range(std::size_t begin, std::size_t end,
                                       std::function<void(std::size_t, std::size_t)> func,
                                       Threading::ThreadPoolAdapter& thread_pool,
                                       std::size_t chunk_size)
        {
            if (begin >= end) return;

            const std::size_t total_size = end - begin;

            // 如果任务量小，单线程执行
            if (total_size <= chunk_size)
            {
                func(begin, end);
                return;
            }

            // 计算需要多少个任务块
            const std::size_t num_chunks = (total_size + chunk_size - 1) / chunk_size;
            std::vector<std::future<void>> futures;
            futures.reserve(num_chunks);

            // 提交所有任务块
            for (std::size_t chunk = 0; chunk < num_chunks; ++chunk)
            {
                std::size_t chunk_begin = begin + chunk * chunk_size;
                std::size_t chunk_end = std::min(chunk_begin + chunk_size, end);

                futures.push_back(
                    thread_pool.Submit([func, chunk_begin, chunk_end]() {
                        func(chunk_begin, chunk_end);
                    })
                );
            }

            // 等待所有任务完成
            for (auto& future : futures)
            {
                future.wait();
            }
        }

    } // namespace Ecs
END_NAMESPACE_COMMON
