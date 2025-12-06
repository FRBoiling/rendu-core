//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_WORK_STEALING_THREAD_POOL_ADVANCED_H
#define RENDU_WORK_STEALING_THREAD_POOL_ADVANCED_H

#include "common/define.h"
#include "work_stealing_thread_pool_interface.h"  // 包含统一接口
#include "common/asio/io_context.h"
#include "thread_pool_stats.h"
#include "common/asio/io_context_strand.h"
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "common/logging/log.h"

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 高级工作窃取线程池 - 继承统一接口
        class AdvancedWorkStealingThreadPool : public IWorkStealingThreadPool
        {
        public:
            explicit AdvancedWorkStealingThreadPool(std::size_t num_threads = std::thread::hardware_concurrency())
                : _thread_count(num_threads)
                , _global_io_context(1)
                , _stop(false)
            {
                _thread_io_contexts.reserve(_thread_count);
                _thread_strands.reserve(_thread_count);
                _thread_workloads.reserve(_thread_count);
                _thread_work_guards.reserve(_thread_count);  // 新增：存储每个线程的work_guard
                
                for (std::size_t i = 0; i < _thread_count; ++i)
                {
                    _thread_io_contexts.emplace_back(std::make_unique<Asio::IoContext>(1));
                    _thread_strands.emplace_back(std::make_unique<Asio::IoContext::Strand>(_thread_io_contexts.back()->make_strand()));
                    _thread_workloads.emplace_back(std::make_unique<std::atomic<uint64_t>>(0));  // 使用 unique_ptr 存储原子变量
                    // 为每个IoContext创建work_guard
                    _thread_work_guards.emplace_back(std::make_unique<Asio::ExecutorWorkGuard>(*_thread_io_contexts.back()));
}

                _threads.reserve(_thread_count);
                
                for (std::size_t i = 0; i < _thread_count; ++i)
                {
                    _threads.emplace_back([this, i] { 
                        WorkerThread(i); 
                    });
                }
                
                _load_balancer_thread = std::thread([this] { LoadBalancerThread(); });
            }

            ~AdvancedWorkStealingThreadPool() override
            {
                Stop();
                Join();
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
                
                if (_load_balancer_thread.joinable())
                {
                    _load_balancer_thread.join();
                }
            }

            void Stop() override
            {
                if (!_stop.exchange(true, std::memory_order_release))
                {
                    _global_io_context.stop();
                    // 先释放所有work_guard，然后停止IoContext
                    for (auto& work_guard : _thread_work_guards)
                    {
                        work_guard.reset();
                    }
                    for (auto& io_ctx : _thread_io_contexts)
                    {
                        io_ctx->stop();
                    }
                }
            }

            // 实现统一接口的性能监控方法
            const ThreadPoolStats& GetStats() const override { return _stats; }
            std::size_t GetThreadCount() const override { return _thread_count; }
            bool IsRunning() const override { return !_stop.load(std::memory_order_acquire); }

        protected:
            // 实现非模板的虚函数，接受类型擦除后的任务
            void SubmitTask(std::function<void()> task) override
            {
                std::size_t target_thread = SelectOptimalThread();
                _thread_workloads[target_thread]->fetch_add(1, std::memory_order_relaxed);  // 使用指针访问原子变量
                
                Asio::Post(*_thread_strands[target_thread], [this, target_thread, task = std::move(task)]() {
                    try {
                        task();
                    } catch (const std::exception& e) {
                        RC_LOG_ERROR("Threading", "Exception in advanced thread pool task: {}", e.what());
                    } catch (...) {
                        RC_LOG_ERROR("Threading", "Unknown exception in advanced thread pool task");
                    }
                    _thread_workloads[target_thread]->fetch_sub(1, std::memory_order_relaxed);  // 使用指针访问原子变量
                });
                
                _stats.total_tasks_processed.fetch_add(1, std::memory_order_relaxed);
            }

        private:
            std::size_t _thread_count;
            Asio::IoContext _global_io_context;
            std::atomic<bool> _stop;
            std::vector<std::thread> _threads;
            std::thread _load_balancer_thread;
            
            std::vector<std::unique_ptr<Asio::IoContext>> _thread_io_contexts;
            std::vector<std::unique_ptr<Asio::IoContext::Strand>> _thread_strands;
            std::vector<std::unique_ptr<std::atomic<uint64_t>>> _thread_workloads;  // 改为 unique_ptr 存储
            std::vector<std::unique_ptr<Asio::ExecutorWorkGuard>> _thread_work_guards;
            ThreadPoolStats _stats;

            // 添加work_guard类型定义
            void WorkerThread(std::size_t index)
            {
                _thread_io_contexts[index]->run();
            }

            void LoadBalancerThread()
            {
                while (!_stop.load(std::memory_order_acquire))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

            std::size_t SelectOptimalThread()
            {
                // 简单的负载均衡算法
                std::size_t min_workload_index = 0;
                uint64_t min_workload = _thread_workloads[0]->load(std::memory_order_relaxed);  // 使用指针访问原子变量
                
                for (std::size_t i = 1; i < _thread_count; ++i)
                {
                    uint64_t workload = _thread_workloads[i]->load(std::memory_order_relaxed);  // 使用指针访问原子变量
                    if (workload < min_workload)
                    {
                        min_workload = workload;
                        min_workload_index = i;
                    }
                }
                
                return min_workload_index;
            }
        };

    } // namespace Threading
END_NAMESPACE_COMMON

#endif //RENDU_WORK_STEALING_THREAD_POOL_ADVANCED_H