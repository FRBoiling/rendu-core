#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include "core/actor/message.h"

BEGIN_NAMESPACE_CORE

/// 消息队列接口
class IMessageQueue {
public:
    virtual ~IMessageQueue() = default;

    /// 推送消息到队列
    virtual void push(std::shared_ptr<Message> msg) = 0;

    /// 从队列取出消息（阻塞）
    virtual std::shared_ptr<Message> pop() = 0;

    /// 尝试从队列取出消息（非阻塞）
    virtual bool try_pop(std::shared_ptr<Message>& msg) = 0;

    /// 获取队列大小
    virtual size_t size() const = 0;

    /// 清空队列
    virtual void clear() = 0;

    /// 停止队列（唤醒所有等待的线程）
    virtual void stop() = 0;
};

/// 优先级消息队列（使用优先队列）
template<typename Compare = std::less<std::shared_ptr<Message>>>
class PriorityMessageQueue : public IMessageQueue {
public:
    PriorityMessageQueue() : stopped_(false) {}

    void push(std::shared_ptr<Message> msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(msg);
        condition_.notify_one();
    }

    std::shared_ptr<Message> pop() override {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty() || stopped_; });

        if (stopped_ && queue_.empty()) {
            return nullptr;
        }

        auto msg = queue_.top();
        queue_.pop();
        return msg;
    }

    bool try_pop(std::shared_ptr<Message>& msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        msg = queue_.top();
        queue_.pop();
        return true;
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) {
            queue_.pop();
        }
    }

    void stop() override {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
        condition_.notify_all();
    }

private:
    std::priority_queue<
        std::shared_ptr<Message>,
        std::vector<std::shared_ptr<Message>>,
        Compare
    > queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stopped_;
};

/// 普通消息队列（FIFO）
class MessageQueue : public IMessageQueue {
public:
    MessageQueue() : stopped_(false) {}

    void push(std::shared_ptr<Message> msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(msg);
        condition_.notify_one();
    }

    std::shared_ptr<Message> pop() override {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty() || stopped_; });

        if (stopped_ && queue_.empty()) {
            return nullptr;
        }

        auto msg = queue_.front();
        queue_.pop();
        return msg;
    }

    bool try_pop(std::shared_ptr<Message>& msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        msg = queue_.front();
        queue_.pop();
        return true;
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<std::shared_ptr<Message>> empty;
        std::swap(queue_, empty);
    }

    void stop() override {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
        condition_.notify_all();
    }

private:
    std::queue<std::shared_ptr<Message>> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stopped_;
};

END_NAMESPACE_CORE
