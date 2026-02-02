#pragma once

#include "common/define.h"
#include "common/io/io_context.h"
#include "common/event/event_define.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>
#include <deque>

BEGIN_NAMESPACE_COMMON
namespace event {

/**
 * @brief 事件处理器包装器
 *
 * 包装单个事件处理器，支持优先级
 */
struct HandlerWrapper {
    EventHandler handler;
    int priority;
    SubscriptionId id;

    HandlerWrapper(EventHandler h, int p, SubscriptionId i)
        : handler(std::move(h)), priority(p), id(i) {}

    bool operator<(const HandlerWrapper& other) const {
        return priority > other.priority; // 优先级高的先执行
    }
};

/**
 * @brief 事件总线
 *
 * 提供事件订阅、发布、取消订阅功能
 * 支持同步和异步分发
 */
class EventBus {
public:
    /**
     * @brief 构造函数
     * @param io I/O 上下文引用
     */
    explicit EventBus(io::IoContext& io);

    ~EventBus();

    // 禁止拷贝和移动
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&) = delete;
    EventBus& operator=(EventBus&&) = delete;

    /**
     * @brief 订阅事件（模板方法）
     * @tparam EventType 事件类型
     * @param handler 事件处理器
     * @return 订阅 ID，用于取消订阅
     */
    template<typename EventType>
    SubscriptionId subscribe(EventHandler handler) {
        static_assert(std::is_base_of<Event, EventType>::value,
                      "EventType must inherit from Event");
        return subscribe_with_id(EventType::static_type(), std::move(handler));
    }

    /**
     * @brief 订阅事件（字符串类型）
     * @param event_type 事件类型字符串
     * @param handler 事件处理器
     * @return 订阅 ID，用于取消订阅
     */
    SubscriptionId subscribe(const std::string& event_type, EventHandler handler);

    /**
     * @brief 订阅事件（带优先级）
     * @param event_type 事件类型字符串
     * @param handler 事件处理器
     * @param priority 优先级（数值越大优先级越高）
     * @return 订阅 ID，用于取消订阅
     */
    SubscriptionId subscribe_with_priority(
        const std::string& event_type,
        EventHandler handler,
        int priority = 0);

    /**
     * @brief 订阅事件（返回 ID）
     * @param event_type 事件类型字符串
     * @param handler 事件处理器
     * @return 订阅 ID，用于取消订阅
     */
    SubscriptionId subscribe_with_id(const std::string& event_type, EventHandler handler);

    /**
     * @brief 取消订阅
     * @param id 订阅 ID
     * @return 是否成功取消
     */
    bool unsubscribe(SubscriptionId id);

    /**
     * @brief 发布事件（同步）
     * @param event 事件对象
     */
    void publish(const Event& event);

    /**
     * @brief 发布事件（异步）
     * @param event 事件对象
     */
    void publish_async(const Event& event);

    /**
     * @brief 发布事件（延迟）
     * @param event 事件对象
     * @param delay_ms 延迟毫秒数
     */
    void publish_delayed(const Event& event, uint32_t delay_ms);

    /**
     * @brief 获取订阅者数量
     * @param event_type 事件类型字符串
     * @return 订阅者数量
     */
    size_t subscriber_count(const std::string& event_type) const;

    /**
     * @brief 清空所有订阅者
     */
    void clear();

    /**
     * @brief 获取 I/O 上下文
     * @return IoContext 引用
     */
    io::IoContext& io_context() { return io_; }

private:
    /**
     * @brief 内部发布事件
     * @param event_type 事件类型字符串
     * @param event 事件对象
     */
    void do_publish(const std::string& event_type, const Event& event);

    /**
     * @brief 生成下一个订阅 ID
     * @return 订阅 ID
     */
    SubscriptionId next_id();

private:
    io::IoContext& io_;
    std::unordered_map<std::string, std::vector<HandlerWrapper>> handlers_;
    std::unordered_map<SubscriptionId, std::pair<std::string, size_t>> subscriptions_;
    mutable std::shared_mutex mutex_;
    std::atomic<SubscriptionId> next_id_;
};

} // namespace event
END_NAMESPACE_COMMON
