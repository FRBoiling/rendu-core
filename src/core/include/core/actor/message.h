#pragma once

#include "core/define.h"
#include <string>
#include <cstdint>
#include <memory>
#include <future>
#include <mutex>
#include <unordered_map>
#include <functional>

BEGIN_NAMESPACE_CORE

/// 消息基类，所有消息都需要继承此类
class Message {
public:
    virtual ~Message() = default;

    /// 获取消息类型标识
    virtual const char* get_type() const = 0;

    // ========== 序列化支持 ==========

    /// 序列化消息为字符串
    /// 格式: type|request_id|data
    virtual std::string serialize() const = 0;

    /// 从字符串反序列化消息数据（子类实现）
    /// data: 序列化后的数据部分（不包含类型和 request_id）
    virtual void deserialize_data(const std::string& data) = 0;

public:
    /// 消息ID（用于追踪和调试）
    uint64_t message_id() const { return message_id_; }
    uint64_t message_id_;  // 公开访问，用于优先级设置和测试

    /// 请求 ID（用于 Ask 模式响应匹配）
    uint64_t request_id() const { return request_id_; }
    void set_request_id(uint64_t id) { request_id_ = id; }
    uint64_t request_id_{0};

protected:
    explicit Message(uint64_t id = 0) : message_id_(id) {}
};

/// 空消息，用于同步等待
class EmptyMessage : public Message {
public:
    EmptyMessage() : Message(0) {}
    const char* get_type() const override { return "EmptyMessage"; }

    std::string serialize() const override {
        return "EmptyMessage|" + std::to_string(request_id_) + "|";
    }

    void deserialize_data(const std::string& data) override {
        // 空消息无数据
    }
};

/// 消息工厂函数类型
using MessageFactory = std::function<std::shared_ptr<Message>()>;

/// 消息类型注册表（用于反序列化）
class MessageRegistry {
public:
    /// 注册消息类型
    static void register_type(const std::string& type, MessageFactory factory);

    /// 创建消息实例
    static std::shared_ptr<Message> create(const std::string& type);

    /// 反序列化消息
    static std::shared_ptr<Message> deserialize(const std::string& data);

    /// 检查类型是否已注册
    static bool is_registered(const std::string& type);

private:
    static std::unordered_map<std::string, MessageFactory>& get_registry();
    static std::mutex registry_mutex_;
};

/// 消息类型注册辅助类（用于静态注册）
template<typename MessageType>
class MessageRegistrar {
public:
    MessageRegistrar(const std::string& type) {
        MessageRegistry::register_type(type, []() {
            return std::make_shared<MessageType>();
        });
    }
};

/// 辅助宏：定义消息类型并自动注册
#define REGISTER_MESSAGE_TYPE(MessageClass) \
    static Rendu::MessageRegistrar<MessageClass> g_##MessageClass##_registrar(#MessageClass)

/// 消息 Promise，用于 Ask 模式
class MessagePromise {
public:
    MessagePromise() = default;

    /// 设置响应值
    void set_value(std::shared_ptr<Message> msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (settled_) return;
        settled_ = true;
        promise_.set_value(msg);
    }

    /// 获取 Future
    std::future<std::shared_ptr<Message>> get_future() {
        return promise_.get_future();
    }

    /// 检查是否已设置
    bool is_settled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return settled_;
    }

private:
    std::promise<std::shared_ptr<Message>> promise_;
    mutable std::mutex mutex_;
    bool settled_{false};
};

END_NAMESPACE_CORE
