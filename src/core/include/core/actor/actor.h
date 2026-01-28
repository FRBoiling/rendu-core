#pragma once

#include <string>
#include <memory>
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <thread>
#include <future>
#include <chrono>

#include "core/define.h"
#include "core/actor/message.h"
#include "core/actor/actor_ref.h"
#include "core/actor/message_queue.h"

BEGIN_NAMESPACE_CORE

/// Actor 基类
/// Actor 是轻量级的并发单元，通过消息通信
class Actor {
public:
    virtual ~Actor() = default;

    /// 处理消息（由 ActorSystem 调用）
    virtual void receive(std::shared_ptr<Message> msg) = 0;

    /// 获取 Actor 名称
    const std::string& name() const { return name_; }

    /// 获取 Actor 路径
    const std::string& path() const { return path_; }

    /// 获取 Actor 引用
    ActorRef self() const { return self_ref_; }

    /// 获取 Actor 状态
    bool is_started() const { return started_; }
    bool is_stopped() const { return stopped_; }

    /// Actor 生命周期回调（子类可选重写）
    virtual void on_start() {}
    virtual void on_stop() {}

protected:
    /// 构造函数
    explicit Actor(const std::string& name)
        : name_(name), started_(false), stopped_(false) {}

public:
    /// 设置路径和引用（由 ActorSystem 调用）
    void setup(const std::string& path, const ActorRef& self_ref) {
        path_ = path;
        self_ref_ = self_ref;
    }

    /// 标记为已启动（由 ActorSystem 调用）
    void mark_started() { started_ = true; }

    /// 标记为已停止（由 ActorSystem 调用）
    void mark_stopped() { stopped_ = true; }

private:
    std::string name_;         // Actor 名称
    std::string path_;         // Actor 路径
    ActorRef self_ref_;        // 自身引用
    std::atomic<bool> started_; // 是否已启动
    std::atomic<bool> stopped_; // 是否已停止
};

/// 线程池，用于复用工作线程
class ActorThreadPool {
public:
    ActorThreadPool(size_t thread_count);
    ~ActorThreadPool();

    /// 提交任务到线程池
    template<typename F>
    void submit(F&& task) {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(std::forward<F>(task));
        condition_.notify_one();
    }

    /// 停止线程池
    void stop();

    /// 获取线程数量
    size_t thread_count() const { return thread_count_; }

private:
    void worker_loop();

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stopped_;
    size_t thread_count_;
};

/// Actor 系统管理器
/// 负责创建、管理、销毁 Actor
class ActorSystem {
public:
    using ActorFactory = std::function<std::unique_ptr<Actor>()>;
    using MessageHandler = std::function<void(std::shared_ptr<Message>)>;

    /// 构造函数，可指定线程池大小
    explicit ActorSystem(size_t thread_pool_size = 4);
    ~ActorSystem();

    /// 启动 Actor 系统
    void start();

    /// 停止 Actor 系统
    void stop();

    /// 创建 Actor（使用工厂函数）
    /// 返回 Actor 引用
    ActorRef create_actor(const std::string& name, ActorFactory factory);

    /// 创建 Actor（直接传入 Actor 实例）
    ActorRef create_actor(std::unique_ptr<Actor> actor);

    /// 查找 Actor
    std::shared_ptr<Actor> find_actor(const ActorRef& ref);

    /// 发送消息（Tell 模式，异步）
    void tell(const ActorRef& target, std::shared_ptr<Message> msg);

    /// 发送消息并等待回复（Ask 模式，支持超时）
    /// timeout_ms: 超时时间（毫秒），0 表示无限等待
    std::shared_ptr<Message> ask(const ActorRef& target, std::shared_ptr<Message> msg,
                                  uint64_t timeout_ms = 0);

    /// 停止 Actor
    void stop_actor(const ActorRef& ref);

    /// 获取 Actor 数量
    size_t actor_count() const;

    /// 设置/获取请求 Promise（用于 Ask 模式，按请求 ID）
    void set_request_promise(uint64_t request_id, std::shared_ptr<MessagePromise> promise);
    std::shared_ptr<MessagePromise> get_request_promise(uint64_t request_id);

    /// 发送响应消息（用于 Ask 模式）
    void reply(uint64_t request_id, std::shared_ptr<Message> response);

private:
    /// 生成唯一 Actor ID
    uint64_t generate_actor_id();

    /// 生成唯一请求 ID（用于 Ask 模式）
    uint64_t generate_request_id();

    /// 获取 Actor 消息队列
    std::shared_ptr<IMessageQueue> get_message_queue(const ActorRef& ref);

    /// Actor 处理函数（由线程池调度）
    void process_messages(const ActorRef& ref);

private:
    std::atomic<bool> running_;
    std::atomic<uint64_t> next_actor_id_;
    std::atomic<uint64_t> next_request_id_;

    // Actor 存储
    std::unordered_map<ActorRef, std::shared_ptr<Actor>, ActorRefHash> actors_;
    std::unordered_map<ActorRef, std::shared_ptr<IMessageQueue>, ActorRefHash> queues_;
    
    // 请求 Promise 存储（按请求 ID）
    std::unordered_map<uint64_t, std::shared_ptr<MessagePromise>> promises_;

    // 线程池（替代每 Actor 一线程）
    std::unique_ptr<ActorThreadPool> thread_pool_;

    mutable std::mutex actors_mutex_;
    mutable std::mutex promises_mutex_;
};

END_NAMESPACE_CORE
