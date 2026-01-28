#include "core/actor/actor.h"
#include <common/log/logger.h>
#include <common/io/io_context.h>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <future>

BEGIN_NAMESPACE_CORE

// 初始化 io_context 用于日志
static std::shared_ptr<Rendu::io::IoContext> g_io_context;
static std::once_flag g_io_context_init_flag;

static void init_logging() {
    // 禁用日志以避免阻塞
    std::call_once(g_io_context_init_flag, []() {
        g_io_context = std::make_shared<Rendu::io::IoContext>(1);
        Rendu::log::init_default_io_context(*g_io_context);
        // 不创建 logger，避免日志系统阻塞
    });
}

// ============================================================================
// ActorThreadPool 实现
// ============================================================================

ActorThreadPool::ActorThreadPool(size_t thread_count)
    : stopped_(false), thread_count_(thread_count) {
    init_logging();

    for (size_t i = 0; i < thread_count_; ++i) {
        threads_.emplace_back(&ActorThreadPool::worker_loop, this);
    }
}

ActorThreadPool::~ActorThreadPool() {
    stop();
}

void ActorThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stopped_) return;
        stopped_ = true;
        condition_.notify_all();
    }

    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads_.clear();
}

void ActorThreadPool::worker_loop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] {
                return stopped_ || !tasks_.empty();
            });

            if (stopped_ && tasks_.empty()) {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                RENDU_LOG_ERROR("ActorThreadPool worker error: {}", e.what());
            }
        }
    }
}

// ============================================================================
// ActorSystem 实现
// ============================================================================

ActorSystem::ActorSystem(size_t thread_pool_size)
    : running_(false), next_actor_id_(1), next_request_id_(1) {
    init_logging();
    thread_pool_ = std::make_unique<ActorThreadPool>(thread_pool_size);
}

ActorSystem::~ActorSystem() {
    if (running_) {
        stop();
    }
}

void ActorSystem::start() {
    if (running_) {
        RENDU_LOG_WARN("ActorSystem already started");
        return;
    }

    running_ = true;
    RENDU_LOG_INFO("ActorSystem started with thread pool (size: {})",
                   thread_pool_->thread_count());
}

void ActorSystem::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    std::lock_guard<std::mutex> lock(actors_mutex_);

    // 停止所有 Actor 和队列
    for (auto& [ref, actor] : actors_) {
        actor->on_stop();
        actor->mark_stopped();

        // 停止消息队列（释放阻塞的 pop 调用）
        auto queue_it = queues_.find(ref);
        if (queue_it != queues_.end()) {
            queue_it->second->stop();
        }
    }

    // 清理资源
    actors_.clear();
    queues_.clear();

    // 清理所有 pending promises
    {
        std::lock_guard<std::mutex> plock(promises_mutex_);
        // 为所有未完成的 promise 设置 nullptr 以避免阻塞
        for (auto& [id, promise] : promises_) {
            if (promise && !promise->is_settled()) {
                promise->set_value(nullptr);
            }
        }
        promises_.clear();
    }

    RENDU_LOG_INFO("ActorSystem stopped");
}

ActorRef ActorSystem::create_actor(const std::string& name, ActorFactory factory) {
    auto actor = factory();
    return create_actor(std::move(actor));
}

ActorRef ActorSystem::create_actor(std::unique_ptr<Actor> actor) {
    if (!actor) {
        RENDU_LOG_ERROR("Failed to create actor: nullptr");
        return ActorRef::invalid();
    }

    uint64_t actor_id = generate_actor_id();
    std::string path = "/user/" + actor->name() + "_" + std::to_string(actor_id);
    ActorRef ref(path, actor_id);

    std::lock_guard<std::mutex> lock(actors_mutex_);

    // 设置 Actor 信息
    actor->setup(path, ref);

    // 创建消息队列
    auto queue = std::make_shared<MessageQueue>();
    queues_[ref] = queue;

    // 存储 Actor
    actors_[ref] = std::shared_ptr<Actor>(actor.release());

    // 启动 Actor
    actors_[ref]->on_start();
    actors_[ref]->mark_started();

    // 提交消息处理任务到线程池
    thread_pool_->submit([this, ref]() {
        process_messages(ref);
    });

    RENDU_LOG_INFO("Created actor: {} ({})", path, actor_id);

    return ref;
}

std::shared_ptr<Actor> ActorSystem::find_actor(const ActorRef& ref) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    auto it = actors_.find(ref);
    return (it != actors_.end()) ? it->second : nullptr;
}

void ActorSystem::tell(const ActorRef& target, std::shared_ptr<Message> msg) {
    if (!target.is_valid()) {
        RENDU_LOG_ERROR("Invalid actor reference for tell");
        return;
    }

    auto queue = get_message_queue(target);
    if (!queue) {
        RENDU_LOG_ERROR("Actor not found: {}", target.to_string());
        return;
    }

    queue->push(msg);
}

std::shared_ptr<Message> ActorSystem::ask(const ActorRef& target,
                                          std::shared_ptr<Message> msg,
                                          uint64_t timeout_ms) {
    if (!target.is_valid()) {
        RENDU_LOG_ERROR("Invalid actor reference for ask");
        return nullptr;
    }

    // 生成唯一请求 ID
    uint64_t request_id = generate_request_id();
    msg->set_request_id(request_id);

    // 创建 Promise
    auto promise = std::make_shared<MessagePromise>();
    auto future = promise->get_future();

    // 注册 Promise（使用请求 ID）
    set_request_promise(request_id, promise);

    // 发送消息
    tell(target, msg);

    // 等待响应
    if (timeout_ms == 0) {
        // 无限等待
        future.wait();
    } else {
        // 带超时等待
        auto status = future.wait_for(std::chrono::milliseconds(timeout_ms));
        if (status == std::future_status::timeout) {
            RENDU_LOG_WARN("Ask timeout: request_id={}, target={}", request_id, target.to_string());
            // 清理 Promise
            {
                std::lock_guard<std::mutex> lock(promises_mutex_);
                promises_.erase(request_id);
            }
            return nullptr;
        }
    }

    // 返回响应
    try {
        return future.get();
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("Ask error: {}", e.what());
        return nullptr;
    }
}

void ActorSystem::stop_actor(const ActorRef& ref) {
    if (!ref.is_valid()) {
        RENDU_LOG_ERROR("Invalid actor reference for stop");
        return;
    }

    bool found = false;

    {
        std::lock_guard<std::mutex> lock(actors_mutex_);

        auto it = actors_.find(ref);
        if (it != actors_.end()) {
            found = true;

            // 停止 Actor
            it->second->on_stop();
            it->second->mark_stopped();

            // 停止消息队列（释放阻塞的 pop 调用）
            auto queue_it = queues_.find(ref);
            if (queue_it != queues_.end()) {
                queue_it->second->stop();
            }
        }
    }

    if (!found) {
        RENDU_LOG_ERROR("Actor not found: {}", ref.to_string());
        return;
    }

    // 清理 Actor 和队列
    {
        std::lock_guard<std::mutex> lock(actors_mutex_);
        actors_.erase(ref);
        queues_.erase(ref);
    }

    // 清理 Promise（按请求 ID 清理所有相关 promises）
    // 注意：由于 promises_ 现在使用请求 ID 作为键，这里不再按 ActorRef 清理
    // 过期的 Promise 会在超时时自动清理

    RENDU_LOG_INFO("Stopped actor: {}", ref.to_string());
}

size_t ActorSystem::actor_count() const {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    return actors_.size();
}

uint64_t ActorSystem::generate_actor_id() {
    return next_actor_id_++;
}

uint64_t ActorSystem::generate_request_id() {
    return next_request_id_++;
}

std::shared_ptr<IMessageQueue> ActorSystem::get_message_queue(const ActorRef& ref) {
    std::lock_guard<std::mutex> lock(actors_mutex_);
    auto it = queues_.find(ref);
    return (it != queues_.end()) ? it->second : nullptr;
}

void ActorSystem::set_request_promise(uint64_t request_id, std::shared_ptr<MessagePromise> promise) {
    std::lock_guard<std::mutex> lock(promises_mutex_);
    promises_[request_id] = promise;
}

std::shared_ptr<MessagePromise> ActorSystem::get_request_promise(uint64_t request_id) {
    std::lock_guard<std::mutex> lock(promises_mutex_);
    auto it = promises_.find(request_id);
    return (it != promises_.end()) ? it->second : nullptr;
}

void ActorSystem::reply(uint64_t request_id, std::shared_ptr<Message> response) {
    auto promise = get_request_promise(request_id);
    if (promise && !promise->is_settled()) {
        // 设置响应的 request_id 以便调试
        response->set_request_id(request_id);
        promise->set_value(response);
    }
}

void ActorSystem::process_messages(const ActorRef& ref) {
    // 获取 actor 和 queue 的引用，不持有锁
    std::shared_ptr<Actor> actor;
    std::shared_ptr<IMessageQueue> queue;

    {
        std::lock_guard<std::mutex> lock(actors_mutex_);
        auto it = actors_.find(ref);
        if (it != actors_.end()) {
            actor = it->second;
        }

        auto queue_it = queues_.find(ref);
        if (queue_it != queues_.end()) {
            queue = queue_it->second;
        }
    }

    if (!actor || !queue) {
        RENDU_LOG_ERROR("Failed to process messages: {}", ref.to_string());
        return;
    }

    RENDU_LOG_INFO("Actor processing started: {}", ref.to_string());

    while (running_ && !actor->is_stopped()) {
        try {
            auto msg = queue->pop();
            if (msg) {
                // 处理消息
                actor->receive(msg);
            } else {
                // 返回 nullptr 表示队列已停止
                break;
            }
        } catch (const std::exception& e) {
            RENDU_LOG_ERROR("Actor processing error ({}): {}", ref.to_string(), e.what());
        }
    }

    RENDU_LOG_INFO("Actor processing stopped: {}", ref.to_string());
}

END_NAMESPACE_CORE
