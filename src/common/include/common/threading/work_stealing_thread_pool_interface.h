//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_WORK_STEALING_THREAD_POOL_INTERFACE_H
#define RENDU_WORK_STEALING_THREAD_POOL_INTERFACE_H

#include "common/define.h"
#include "thread_pool_stats.h"
#include <functional>
#include <future>
#include <memory>
#include <thread>

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 统一的工作窃取线程池接口
        class IWorkStealingThreadPool
        {
        public:
            virtual ~IWorkStealingThreadPool() = default;

            // 核心API - 所有实现必须提供
            template <typename F, typename... Args>
            auto Submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
            {
                using return_type = std::invoke_result_t<F, Args...>;
                
                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );
                
                std::future<return_type> result = task->get_future();
                
                // 使用类型擦除将任务包装为 std::function<void()>
                SubmitTask([task]() { (*task)(); });
                
                return result;
            }

            // 向后兼容的API
            template <typename T>
            void PostWork(T&& work)
            {
                Submit(std::forward<T>(work));
            }

            // 生命周期管理
            virtual void Join() = 0;
            virtual void Stop() = 0;

            // 性能监控API
            virtual const ThreadPoolStats& GetStats() const = 0;
            virtual std::size_t GetThreadCount() const = 0;
            virtual bool IsRunning() const = 0;

            // 获取底层实现（用于高级功能）
            virtual void* GetUnderlyingImpl() { return nullptr; }

        protected:
            // 非模板的虚函数，接受类型擦除后的任务
            virtual void SubmitTask(std::function<void()> task) = 0;
        };

        // 线程池类型枚举
        enum class WorkStealingThreadPoolType
        {
            Basic,      // 基础工作窃取算法
            IO,         // 基于 io_context 的实现
            Advanced    // 高级负载均衡实现
        };



    } // namespace Threading
END_NAMESPACE_COMMON

#endif //RENDU_WORK_STEALING_THREAD_POOL_INTERFACE_H