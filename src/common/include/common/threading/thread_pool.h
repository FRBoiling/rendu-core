//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_THREAD_POOL_H
#define RENDU_THREAD_POOL_H

#include "common/define.h"
#include <future>
#include <thread>

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 线程池实现
        class ThreadPool
        {
        public:
            ThreadPool(size_t num_threads);
            ~ThreadPool();

            // 提交任务到线程池
            template <typename F, typename... Args>
            auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

            // 等待所有任务完成
            void wait_for_all();

        private:
            std::vector<std::thread> workers_;
            std::queue<std::function<void()>> tasks_;

            std::mutex queue_mutex_;
            std::condition_variable condition_;
            bool stop_;
            std::atomic<size_t> active_tasks_;
            std::mutex wait_mutex_;
            std::condition_variable wait_condition_;
        };

        template <typename F, typename... Args>
        auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
        {
            using return_type = std::invoke_result_t<F, Args...>;
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            std::future<return_type> result = task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                if (stop_)
                {
                    throw std::runtime_error("Cannot submit task to stopped ThreadPool");
                }
                tasks_.emplace([task]() { (*task)(); });
            }
            condition_.notify_one();
            return result;
        }
    } // namespace threading
END_NAMESPACE_COMMON

#endif //RENDU_THREAD_POOL_H
