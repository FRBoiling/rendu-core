//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_WORK_STEALING_THREAD_POOL_IO_H
#define RENDU_WORK_STEALING_THREAD_POOL_IO_H

#include "common/define.h"
#include "work_stealing_thread_pool_interface.h"  // 包含统一接口
#include "common/asio/io_context.h"
#include "thread_pool_stats.h"
#include "common/logging/log.h"
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "common/asio/post.h"
#include "common/asio/executor_work_guard.h"

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 基于 io_context 的工作窃取线程池实现 - 继承统一接口
        class WorkStealingThreadPoolIO : public IWorkStealingThreadPool
        {
        public:
            explicit WorkStealingThreadPoolIO(std::size_t num_threads = std::thread::hardware_concurrency())
                : _thread_count(num_threads)
                , _io_context(num_threads)
                , _stop(false)
            {
                _threads.reserve(_thread_count);

                // 创建work_guard防止IoContext在没有任务时停止
                _work_guard = std::make_unique<Asio::ExecutorWorkGuard>(_io_context);

                for (std::size_t i = 0; i < _thread_count; ++i)
                {
                    _threads.emplace_back([this] { 
                        _io_context.run();
                    });
                }
                
                RC_LOG_INFO("Threading", "WorkStealingThreadPoolIO started with {} threads", _thread_count);
            }

            ~WorkStealingThreadPoolIO() override
            {
                Stop();
                Join();
            }

            void Stop() override
            {
                if (!_stop.exchange(true, std::memory_order_release))
                {
                    // 先释放work_guard，然后停止IoContext
                    _work_guard.reset();
                    _io_context.stop();
                    RC_LOG_INFO("Threading", "WorkStealingThreadPoolIO stopped");
                }
            }

            // 实现统一接口的生命周期方法
            void Join() override
            {
                if (!_stop.load(std::memory_order_acquire))
                {
                    Stop();
                }
                for (auto& thread : _threads)
                {
                    if (thread.joinable())
                    {
                        thread.join();
                    }
                }
                
                RC_LOG_INFO("Threading", "WorkStealingThreadPoolIO joined all threads");
            }

            // 实现统一接口的性能监控方法
            const ThreadPoolStats& GetStats() const override { return _stats; }
            std::size_t GetThreadCount() const override { return _thread_count; }
            bool IsRunning() const override { return !_stop.load(std::memory_order_acquire); }

            // 获取底层 io_context（高级功能）
            void* GetUnderlyingImpl() override { return &_io_context; }

        protected:
            // 实现非模板的虚函数，接受类型擦除后的任务
            void SubmitTask(std::function<void()> task) override
            {
                Asio::Post(_io_context, [task]() {
                    try {
                        task();
                    } catch (const std::exception& e) {
                        RC_LOG_ERROR("Threading", "Exception in thread pool task: {}", e.what());
                    } catch (...) {
                        RC_LOG_ERROR("Threading", "Unknown exception in thread pool task");
                    }
                });
                
                _stats.total_tasks_processed.fetch_add(1, std::memory_order_relaxed);
                RC_LOG_DEBUG("Threading", "Task submitted to thread pool, total processed: {}", 
                           _stats.total_tasks_processed.load());
            }

        private:
            std::size_t _thread_count;
            Asio::IoContext _io_context;
            std::atomic<bool> _stop;
            std::vector<std::thread> _threads;
            ThreadPoolStats _stats;

            std::unique_ptr<Asio::ExecutorWorkGuard> _work_guard;
        };

    } // namespace Threading
END_NAMESPACE_COMMON

#endif //RENDU_WORK_STEALING_THREAD_POOL_IO_H