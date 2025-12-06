#pragma once

#include "core/define.h"
#include "common/ecs/i_system.h"
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <memory>

#include "common/asio/io_context.h"
#include "common/threading/thread_pool_adapter.h"

BEGIN_NAMESPACE_CORE
    struct AsyncEvent
    {
        enum class Type
        {
            FILE_IO,
            NETWORK,
            TIMER,
            CUSTOM
        };

        Type type;
        std::function<void()> handler;
        std::chrono::steady_clock::time_point scheduled_time;

        AsyncEvent(Type t, std::function<void()> h,
                   std::chrono::steady_clock::time_point st = std::chrono::steady_clock::now())
            : type(t), handler(std::move(h)), scheduled_time(st) {}

        bool operator<(const AsyncEvent& other) const {
            return scheduled_time > other.scheduled_time;
        }
    };

    class AsyncEventSystem : public Ecs::ISystem
    {
    public:
        AsyncEventSystem()
            : thread_pool_(std::make_unique<Threading::ThreadPoolAdapter>(Threading::WorkStealingThreadPoolType::IO, std::thread::hardware_concurrency()))
        {
        }

        // 配置阶段：系统配置（并行）
        void configure(Ecs::World* world) override;

        // 初始化阶段：系统初始化（并行）
        void initialize() override;

        // 更新阶段（有序）：游戏逻辑（有序执行）
        void update_sequential(float delta_time) override;

        // 更新阶段（并行）：同步/清理（并行执行）
        void update_parallel(float delta_time) override;

        // 关闭阶段：系统关闭（并行）
        void shutdown() override;

        // 清理阶段：资源清理（有序）
        void cleanup() override;

        // 系统信息
        std::string get_name() const override;
        std::vector<std::string> get_dependencies() const override;
        Ecs::SystemExecutionMode get_execution_mode() const override;

        // 事件管理
        void post_event(AsyncEvent::Type type, std::function<void()> handler);
        void schedule_event(AsyncEvent::Type type, std::function<void()> handler,
                            std::chrono::milliseconds delay);
        bool HasPendingEvents() const;

        // 统计信息
        size_t get_pending_events() const;
        size_t get_processed_events() const;

        // 获取IoContext引用
        Asio::IoContext& get_io_context() { return io_context_; }

    private:
        void process_events();
        void process_scheduled_events();
        void start_io_context();

        Asio::IoContext io_context_;
        std::unique_ptr<Threading::ThreadPoolAdapter> thread_pool_;
        std::atomic<bool> running_{false};

        // 队列管理
        std::queue<AsyncEvent> immediate_queue_;
        std::priority_queue<AsyncEvent> scheduled_queue_;
        mutable std::mutex immediate_mutex_;
        mutable std::mutex scheduled_mutex_;

        // 统计信息
        std::atomic<size_t> processed_events_{0};
        std::atomic<size_t> pending_events_{0};
    };

END_NAMESPACE_CORE
