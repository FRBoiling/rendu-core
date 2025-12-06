#include "core/async_event/async_event_system.h"
#include "common/logging/log.h"
#include <thread>
#include <algorithm>

#include "common/asio/post.h"
#include "common/asio/executor_work_guard.h"
#include "common/ecs/world.h"
#include "common/threading/thread_pool_adapter.h"

BEGIN_NAMESPACE_CORE
    using namespace Ecs;

    void AsyncEventSystem::configure(World* world)
    {
        world_ = world;
    }

    std::string AsyncEventSystem::get_name() const
    {
        return "AsyncEventSystem";
    }

    std::vector<std::string> AsyncEventSystem::get_dependencies() const
    {
        return {}; // 无依赖，可以与其他系统并行初始化
    }

    Ecs::SystemExecutionMode AsyncEventSystem::get_execution_mode() const
    {
        return Ecs::SystemExecutionMode::PARALLEL; // 支持并行执行
    }

    void AsyncEventSystem::initialize()
    {
        if (initialized_)
        {
            return;
        }

        running_ = true;
        start_io_context();

        // 启动事件处理线程
        thread_pool_->PostWork([this]()
        {
            while (running_)
            {
                process_events();
                process_scheduled_events();

                if (!HasPendingEvents())
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        });

        initialized_ = true;
        RC_LOG_DEBUG("application", "AsyncEventSystem initialized");
    }

    void AsyncEventSystem::update_sequential(float delta_time)
    {
        static auto last_check_time = std::chrono::steady_clock::now();
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_check_time);

        if (elapsed.count() > 1000)
        {
            // 每秒检查一次
            const auto& stats = thread_pool_->GetStats();
            double tasks_per_second = stats.GetTasksPerSecond();

            RC_LOG_INFO("async_event_system",
                        "Performance stats - Tasks/sec: {:.1f}, Queue: {}/{}",
                        tasks_per_second, stats.queue_size.load(), stats.max_queue_size.load());

            last_check_time = current_time;
        }

        if (delta_time > 0.1f)
        {
            RC_LOG_WARN("application", "High latency detected: {:.3f}s, pending events: {}, processed: {}",
                        delta_time, get_pending_events(), get_processed_events());
        }
    }

    void AsyncEventSystem::update_parallel(float delta_time)
    {
        // 并行阶段不需要特殊处理
    }

    void AsyncEventSystem::shutdown()
    {
        if (!initialized_)
        {
            return;
        }

        RC_LOG_INFO("application", "AsyncEventSystem shutdown");

        running_ = false;
        io_context_.stop();

        initialized_ = false;
    }

    void AsyncEventSystem::cleanup()
    {
        // 清空队列
        {
            std::lock_guard<std::mutex> lock(immediate_mutex_);
            while (!immediate_queue_.empty())
            {
                immediate_queue_.pop();
            }
        }

        {
            std::lock_guard<std::mutex> lock(scheduled_mutex_);
            while (!scheduled_queue_.empty())
            {
                scheduled_queue_.pop();
            }
        }

        pending_events_ = 0;
    }

    bool AsyncEventSystem::HasPendingEvents() const
    {
        return !immediate_queue_.empty() || !scheduled_queue_.empty();
    }

    void AsyncEventSystem::post_event(AsyncEvent::Type type, std::function<void()> handler)
    {
        AsyncEvent event(type, std::move(handler));

        {
            std::lock_guard<std::mutex> lock(immediate_mutex_);
            immediate_queue_.push(std::move(event));
            pending_events_++;
        }
    }

    void AsyncEventSystem::schedule_event(AsyncEvent::Type type, std::function<void()> handler,
                                          std::chrono::milliseconds delay)
    {
        auto scheduled_time = std::chrono::steady_clock::now() + delay;
        AsyncEvent event(type, std::move(handler), scheduled_time);

        {
            std::lock_guard<std::mutex> lock(scheduled_mutex_);
            scheduled_queue_.push(std::move(event));
            pending_events_++;
        }
    }

    size_t AsyncEventSystem::get_pending_events() const
    {
        return pending_events_.load();
    }

    size_t AsyncEventSystem::get_processed_events() const
    {
        return processed_events_.load();
    }

    void AsyncEventSystem::process_events()
    {
        std::queue<AsyncEvent> local_queue;

        // 快速交换队列内容，减少锁持有时间
        {
            std::lock_guard<std::mutex> lock(immediate_mutex_);
            if (immediate_queue_.empty())
            {
                return;
            }
            std::swap(local_queue, immediate_queue_);
        }

        // 处理所有立即执行的事件
        while (!local_queue.empty())
        {
            auto event = std::move(local_queue.front());
            local_queue.pop();

            try
            {
                // 使用IoContext异步执行事件
                Asio::Post(io_context_, [this, event = std::move(event)]() mutable
                {
                    try
                    {
                        event.handler();
                        processed_events_++;
                        pending_events_--;
                    }
                    catch (const std::exception& e)
                    {
                        RC_LOG_ERROR("async_event_system", "Event handler error: {}", e.what());
                    }
                });
            }
            catch (const std::exception& e)
            {
                RC_LOG_ERROR("async_event_system", "Failed to post event: {}", e.what());
                pending_events_--; // 即使失败也要减少计数
            }
        }
    }

    void AsyncEventSystem::process_scheduled_events()
    {
        auto now = std::chrono::steady_clock::now();
        std::vector<AsyncEvent> ready_events;

        {
            std::lock_guard<std::mutex> lock(scheduled_mutex_);

            // 检查所有到期的定时事件
            while (!scheduled_queue_.empty())
            {
                const auto& next_event = scheduled_queue_.top();
                if (next_event.scheduled_time <= now)
                {
                    ready_events.push_back(next_event);
                    scheduled_queue_.pop();
                }
                else
                {
                    break; // 队列按时间排序，后面的都未到期
                }
            }
        }

        // 处理到期的定时事件
        for (auto& event : ready_events)
        {
            try
            {
                Asio::Post(io_context_, [this, event = std::move(event)]() mutable
                {
                    try
                    {
                        event.handler();
                        processed_events_++;
                        pending_events_--;
                    }
                    catch (const std::exception& e)
                    {
                        RC_LOG_ERROR("async_event_system", "Scheduled event handler error: {}", e.what());
                    }
                });
            }
            catch (const std::exception& e)
            {
                RC_LOG_ERROR("async_event_system", "Failed to post scheduled event: {}", e.what());
                pending_events_--;
            }
        }
    }

    void AsyncEventSystem::start_io_context()
    {
        // 在线程池中运行IoContext - 使用封装的Asio::Work类
        thread_pool_->PostWork([this]()
        {
            try
            {
                Asio::ExecutorWorkGuard work(io_context_); // 使用封装的Work类
                while (running_)
                {
                    try
                    {
                        io_context_.run();
                        break; // 正常退出
                    }
                    catch (const std::exception& e)
                    {
                        RC_LOG_ERROR("async_event_system", "IoContext error: {}", e.what());
                        // 短暂延迟后重试
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
            }
            catch (const std::exception& e)
            {
                RC_LOG_ERROR("async_event_system", "IoContext worker error: {}", e.what());
            }
        });
    }

END_NAMESPACE_CORE
