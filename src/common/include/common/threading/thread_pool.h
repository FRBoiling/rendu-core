//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_THREAD_POOL_H
#define RENDU_THREAD_POOL_H

#include "common/define.h"
#include <future>
#include <thread>

#include <asio/post.hpp>
#include <asio/thread_pool.hpp>

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        class ThreadPool
        {
        public:
            explicit ThreadPool(std::size_t numThreads = std::thread::hardware_concurrency()) : _impl(numThreads)
            {
            }

            template <typename T>
            void PostWork(T&& work)
            {
                asio::post(_impl, std::forward<T>(work));
            }

            void Join()
            {
                _impl.join();
            }

            void Stop()
            {
                _impl.stop();
            }

        private:
            asio::thread_pool _impl;
        };
    } // namespace threading
END_NAMESPACE_COMMON

#endif //RENDU_THREAD_POOL_H
