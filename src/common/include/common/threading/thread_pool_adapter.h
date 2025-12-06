//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_THREAD_POOL_ADAPTER_H
#define RENDU_THREAD_POOL_ADAPTER_H

#include "common/define.h"
#include "common/threading/work_stealing_thread_pool_interface.h"  // 包含统一接口
#include "common/threading/work_stealing_thread_pool_factory.h"   // 包含工厂函数
#include "common/threading/thread_pool.h"
#include <future>
#include <functional>
#include <memory>

#define RENDU_USE_WORK_STEALING_THREAD_POOL

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        class ThreadPoolAdapter
        {
        public:
            explicit ThreadPoolAdapter(std::size_t num_threads = std::thread::hardware_concurrency())
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                : _impl(CreateWorkStealingThreadPool(WorkStealingThreadPoolType::Advanced, num_threads))
#else
                : _impl(num_threads)
#endif
            {
            }

            // 支持指定线程池类型的构造函数
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
            explicit ThreadPoolAdapter(WorkStealingThreadPoolType type, std::size_t num_threads = std::thread::hardware_concurrency())
                : _impl(CreateWorkStealingThreadPool(type, num_threads))
            {
            }
#endif

            // 新增：启动线程池（对于IO和Advanced类型需要显式启动）
            void Start()
            {
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                // IO和Advanced类型线程池在构造函数中已经启动了工作线程
                // 这里不需要额外操作，因为它们的IoContext已经在工作线程中运行
#else
                // 传统线程池不需要特殊启动
#endif
            }

            template <typename T>
            void PostWork(T&& work)
            {
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                _impl->PostWork(std::forward<T>(work));
#else
                _impl.PostWork(std::forward<T>(work));
#endif
            }

            template <typename F, typename... Args>
            auto Submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
            {
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                return _impl->Submit(std::forward<F>(f), std::forward<Args>(args)...);
#else
                // 为传统线程池实现Submit方法
                using return_type = std::invoke_result_t<F, Args...>;
                
                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );
                
                std::future<return_type> result = task->get_future();
                
                // 使用PostWork提交任务
                _impl.PostWork([task]() { (*task)(); });
                
                return result;
#endif
            }

            void Join()
            {
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                _impl->Join();
#else
                _impl.Join();
#endif
            }

            void Stop()
            {
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                _impl->Stop();
#else
                _impl.Stop();
#endif
            }

            // 性能监控API（仅在工作窃取模式下可用）
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
            const ThreadPoolStats& GetStats() const { return _impl->GetStats(); }
#else
            // 为传统线程池提供空实现
            struct DummyStats {
                uint64_t total_tasks_processed = 0;
                uint64_t tasks_stolen = 0;
                uint64_t tasks_executed = 0;
                uint64_t queue_size = 0;
                uint64_t max_queue_size = 0;
                double GetUptimeSeconds() const { return 0.0; }
                double GetTasksPerSecond() const { return 0.0; }
            };
            DummyStats GetStats() const { return DummyStats{}; }
#endif
            
            bool IsRunning() const 
            { 
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
                return _impl->IsRunning(); 
#else
                // 传统线程池没有IsRunning方法，返回true表示运行中
                return true;
#endif
            }

        private:
#ifdef RENDU_USE_WORK_STEALING_THREAD_POOL
           std::unique_ptr<IWorkStealingThreadPool> _impl;  // 改为智能指针
#else
            ThreadPool _impl;
#endif
        };

    } // namespace Threading
END_NAMESPACE_COMMON

#endif //RENDU_THREAD_POOL_ADAPTER_H