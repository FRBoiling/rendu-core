//
// Created by 沸腾 on 2025/10/24.
//

#include "common/threading/thread_pool.h"

using namespace common::Threading;
// ThreadPool模板实现

ThreadPool::ThreadPool(size_t num_threads)
    : stop_(false), active_tasks_(0)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        workers_.emplace_back([this]
        {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    this->condition_.wait(lock, [this]
                    {
                        return this->stop_ || !this->tasks_.empty();
                    });
                    if (this->stop_ && this->tasks_.empty())
                    {
                        return;
                    }
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }

                active_tasks_++;
                task();
                active_tasks_--;

                {
                    // 通知等待线程可能有任务完成
                    std::unique_lock<std::mutex> lock(wait_mutex_);
                    wait_condition_.notify_one();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for (std::thread& worker : workers_)
    {
        worker.join();
    }
}



void ThreadPool::wait_for_all()
{
    std::unique_lock<std::mutex> lock(wait_mutex_);
    wait_condition_.wait(lock, [this]
    {
        std::lock_guard<std::mutex> queue_lock(queue_mutex_);
        return tasks_.empty() && active_tasks_ == 0;
    });
}