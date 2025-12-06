//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_WORK_STEALING_THREAD_POOL_H
#define RENDU_WORK_STEALING_THREAD_POOL_H

#include "common/define.h"
#include "work_stealing_thread_pool_interface.h"  // 包含统一接口
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "thread_pool_stats.h"

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 工作窃取线程池实现 - 继承统一接口
        class WorkStealingThreadPool : public IWorkStealingThreadPool
        {
        public:
            explicit WorkStealingThreadPool(std::size_t num_threads = std::thread::hardware_concurrency())
                : _thread_count(num_threads)
                , _stop(false)
            {
                // 初始化队列和互斥锁
                _queues.resize(_thread_count);
                _queue_mutexes.reserve(_thread_count);
                
                // 为每个队列创建互斥锁
                for (std::size_t i = 0; i < _thread_count; ++i)
                {
                    _queue_mutexes.emplace_back(std::make_unique<std::mutex>());
                }
                
                _threads.reserve(_thread_count);
                
                // 创建并启动工作线程
                for (std::size_t i = 0; i < _thread_count; ++i)
                {
                    _threads.emplace_back([this, i] { WorkerThread(i); });
                }
            }

            ~WorkStealingThreadPool() override
            {
                Stop();
                Join();  // 确保所有线程都完成
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
            }

            void Stop() override
            {
                _stop.store(true, std::memory_order_release);
                {
                    std::lock_guard<std::mutex> lock(_global_mutex);
                    _condition.notify_all();
                }
            }

            const ThreadPoolStats& GetStats() const override { return _stats; }
            std::size_t GetThreadCount() const override { return _thread_count; }
            bool IsRunning() const override { return !_stop.load(std::memory_order_acquire); }

        protected:
            // 实现非模板的虚函数，接受类型擦除后的任务
            void SubmitTask(std::function<void()> task) override
            {
                // 选择当前线程的队列（如果在线程池线程中）或随机队列
                std::size_t index = GetThreadIndex();
                {
                    std::lock_guard<std::mutex> lock(*_queue_mutexes[index]);
                    _queues[index].emplace_back(std::move(task));
                }
                
                _stats.total_tasks_processed.fetch_add(1, std::memory_order_relaxed);
                _stats.queue_size.fetch_add(1, std::memory_order_relaxed);
                
                // 更新最大队列大小
                uint64_t current_size = _stats.queue_size.load();
                uint64_t max_size = _stats.max_queue_size.load();
                while (current_size > max_size && 
                       !_stats.max_queue_size.compare_exchange_weak(max_size, current_size))
                {
                    current_size = _stats.queue_size.load();
                }
                
                {
                    std::lock_guard<std::mutex> lock(_global_mutex);
                    _condition.notify_one();
                }
            }

        private:
            std::size_t _thread_count;
            std::atomic<bool> _stop;
            std::vector<std::thread> _threads;
            std::vector<std::deque<std::function<void()>>> _queues;
            std::vector<std::unique_ptr<std::mutex>> _queue_mutexes;
            std::mutex _global_mutex;
            std::condition_variable _condition;
            ThreadPoolStats _stats;
            
            static thread_local std::size_t _thread_index;
            static thread_local bool _is_worker_thread;

            std::size_t GetThreadIndex()
            {
                if (_is_worker_thread)
                {
                    return _thread_index;
                }
                static std::atomic<std::size_t> counter{0};
                return counter.fetch_add(1, std::memory_order_relaxed) % _thread_count;
            }

            // 修改work_stealing_thread_pool.h中的WorkerThread方法
            void WorkerThread(std::size_t index)
            {
                _thread_index = index;
                _is_worker_thread = true;

                while (!_stop.load(std::memory_order_acquire))
                {
                    std::function<void()> task;

                    // 首先尝试从自己的队列获取任务
                    if (PopTaskFromQueue(index, task))
                    {
                        task();
                        _stats.tasks_executed.fetch_add(1, std::memory_order_relaxed);
                        _stats.queue_size.fetch_sub(1, std::memory_order_relaxed);
                        continue;
                    }

                    // 如果自己的队列为空，尝试从其他队列窃取任务
                    bool stolen = false;
                    for (std::size_t i = 0; i < _thread_count; ++i)
                    {
                        if (i == index) continue;

                        if (StealTaskFromQueue(i, task))
                        {
                            task();
                            _stats.tasks_executed.fetch_add(1, std::memory_order_relaxed);
                            _stats.tasks_stolen.fetch_add(1, std::memory_order_relaxed);
                            _stats.queue_size.fetch_sub(1, std::memory_order_relaxed);
                            stolen = true;
                            break;
                        }
                    }

                    if (stolen) continue;

                    // 优化：使用更短的等待时间，避免虚假唤醒
                    std::unique_lock<std::mutex> lock(_global_mutex);
                    if (!_stop.load(std::memory_order_acquire) && !HasAnyTask())
                    {
                        _condition.wait_for(lock, std::chrono::microseconds(100), // 改为100微秒
                                          [this] { return _stop.load(std::memory_order_acquire) || HasAnyTask(); });
                    }
                }
            }

            bool PopTaskFromQueue(std::size_t index, std::function<void()>& task)
            {
                std::lock_guard<std::mutex> lock(*_queue_mutexes[index]);
                if (!_queues[index].empty())
                {
                    task = std::move(_queues[index].back());
                    _queues[index].pop_back();
                    return true;
                }
                return false;
            }

            bool StealTaskFromQueue(std::size_t index, std::function<void()>& task)
            {
                std::lock_guard<std::mutex> lock(*_queue_mutexes[index]);
                if (!_queues[index].empty())
                {
                    task = std::move(_queues[index].front());
                    _queues[index].pop_front();
                    return true;
                }
                return false;
            }

            // 添加辅助函数检查是否有任何任务
            bool HasAnyTask() const
            {
                for (std::size_t i = 0; i < _thread_count; ++i)
                {
                    std::lock_guard<std::mutex> lock(*_queue_mutexes[i]);
                    if (!_queues[i].empty())
                    {
                        return true;
                    }
                }
                return false;
            }
        };

    } // namespace Threading
END_NAMESPACE_COMMON

#endif //RENDU_WORK_STEALING_THREAD_POOL_H