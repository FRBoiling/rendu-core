//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_THEAD_POOL_H
#define RENDU_THEAD_POOL_H

#include "common/define.h"
#include <future>

BEGIN_NAMESPACE_COMMON
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

END_NAMESPACE_COMMON

#endif //RENDU_THEAD_POOL_H