#pragma once

#include <memory>
#include <stack>
#include <mutex>
#include "core/actor/message.h"
#include "core/define.h"

BEGIN_NAMESPACE_CORE

/**
 * @brief Actor 消息内存池
 *
 * 复用消息对象,减少频繁的内存分配。
 * 线程安全,支持多线程并发访问。
 */
class MessagePool {
public:
    static MessagePool& instance();

    /**
     * @brief 分配消息
     * @tparam T 消息类型
     * @param args 消息构造参数
     * @return Message* 消息指针
     */
    template<typename T, typename... Args>
    Message* allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);

        // 尝试从空闲列表获取
        if (!free_list_.empty()) {
            auto msg = free_list_.top();
            free_list_.pop();
            new (msg) T(std::forward<Args>(args)...);
            return msg;
        }

        // 新建消息
        ++total_allocated_;
        return new T(std::forward<Args>(args)...);
    }

    /**
     * @brief 释放消息
     * @param msg 消息指针
     */
    void deallocate(Message* msg) {
        std::lock_guard<std::mutex> lock(mutex_);

        // 调用析构函数
        msg->~Message();

        // 放回空闲列表
        free_list_.push(msg);
    }

    /**
     * @brief 清理空闲列表
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);

        while (!free_list_.empty()) {
            delete free_list_.top();
            free_list_.pop();
        }
    }

    /**
     * @brief 获取空闲列表大小
     */
    size_t free_size() const {
        return free_list_.size();
    }

    /**
     * @brief 获取已分配消息总数
     */
    size_t total_allocated() const {
        return total_allocated_;
    }

private:
    MessagePool() = default;
    ~MessagePool() {
        clear();
    }

    std::stack<Message*> free_list_;
    mutable std::mutex mutex_;
    size_t total_allocated_{0};
};

/**
 * @brief 消息智能指针包装
 */
template<typename T>
class MessagePtr {
public:
    explicit MessagePtr(T* ptr) : ptr_(ptr) {}

    ~MessagePtr() {
        if (ptr_) {
            MessagePool::instance().deallocate(ptr_);
        }
    }

    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }

    // 禁止拷贝
    MessagePtr(const MessagePtr&) = delete;
    MessagePtr& operator=(const MessagePtr&) = delete;

    // 支持移动
    MessagePtr(MessagePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    MessagePtr& operator=(MessagePtr&& other) noexcept {
        if (this != &other) {
            if (ptr_) {
                MessagePool::instance().deallocate(ptr_);
            }
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    bool operator==(std::nullptr_t) const {
        return ptr_ == nullptr;
    }

    bool operator!=(std::nullptr_t) const {
        return ptr_ != nullptr;
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    T* ptr_;
};

END_NAMESPACE_CORE

