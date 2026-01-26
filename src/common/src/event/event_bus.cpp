#include "common/event/event_bus.h"
#include <algorithm>
#include <chrono>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

BEGIN_NAMESPACE_COMMON
namespace event {

EventBus::EventBus(io::IoContext& io)
    : io_(io)
    , next_id_(1) {
}

EventBus::~EventBus() {
    clear();
}

SubscriptionId EventBus::subscribe(const std::string& event_type, EventHandler handler) {
    return subscribe_with_priority(event_type, std::move(handler), 0);
}

SubscriptionId EventBus::subscribe_with_priority(
    const std::string& event_type,
    EventHandler handler,
    int priority) {

    SubscriptionId id = next_id();

    std::unique_lock<std::shared_mutex> lock(mutex_);

    // 添加处理器到列表
    auto& handlers = handlers_[event_type];
    handlers.emplace_back(std::move(handler), priority, id);

    // 按优先级排序（高优先级在前）
    std::sort(handlers.begin(), handlers.end());

    // 记录订阅
    subscriptions_[id] = {event_type, handlers.size() - 1};

    return id;
}

SubscriptionId EventBus::subscribe_with_id(const std::string& event_type, EventHandler handler) {
    return subscribe_with_priority(event_type, std::move(handler), 0);
}

bool EventBus::unsubscribe(SubscriptionId id) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = subscriptions_.find(id);
    if (it == subscriptions_.end()) {
        return false;
    }

    const std::string& event_type = it->second.first;
    size_t index = it->second.second;

    auto handlers_it = handlers_.find(event_type);
    if (handlers_it == handlers_.end()) {
        subscriptions_.erase(it);
        return false;
    }

    auto& handlers = handlers_it->second;
    if (index >= handlers.size()) {
        subscriptions_.erase(it);
        return false;
    }

    // 标记为无效（使用空的 handler）
    handlers[index].handler = nullptr;

    subscriptions_.erase(it);

    return true;
}

void EventBus::publish(const Event& event) {
    do_publish(event.type(), event);
}

void EventBus::publish_async(const Event& event) {
    std::string event_type = event.type();

    // 复制事件数据（需要多态拷贝）
    struct EventCopy {
        std::string type;
        std::shared_ptr<Event> data;

        EventCopy(const std::string& t, const Event& e)
            : type(t), data(e.clone()) {}
    };

    io_.post([this, event_type, event_copy = std::make_shared<EventCopy>(event_type, event)]() {
        if (event_copy->data) {
            do_publish(event_copy->type, *event_copy->data);
        }
    });
}

void EventBus::publish_delayed(const Event& event, uint32_t delay_ms) {
    std::string event_type = event.type();

    // 使用 timer 实现延迟发布
    auto timer = std::make_shared<boost::asio::deadline_timer>(io_.native());
    timer->expires_from_now(boost::posix_time::milliseconds(delay_ms));

    // 克隆事件以在异步回调中使用
    auto event_copy = event.clone();

    timer->async_wait([this, event_type, timer, event_copy](const boost::system::error_code& ec) {
        if (!ec && event_copy) {
            do_publish(event_type, *event_copy);
        }
    });
}

size_t EventBus::subscriber_count(const std::string& event_type) const {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = handlers_.find(event_type);
    if (it == handlers_.end()) {
        return 0;
    }

    // 计算有效的处理器数量
    size_t count = 0;
    for (const auto& wrapper : it->second) {
        if (wrapper.handler) {
            ++count;
        }
    }

    return count;
}

void EventBus::clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    handlers_.clear();
    subscriptions_.clear();
}

void EventBus::do_publish(const std::string& event_type, const Event& event) {
    // 复制处理器列表以避免在锁中执行回调
    std::vector<EventHandler> handlers_to_call;

    {
        std::unique_lock<std::shared_mutex> lock(mutex_);

        auto it = handlers_.find(event_type);
        if (it != handlers_.end()) {
            for (const auto& wrapper : it->second) {
                if (wrapper.handler) {
                    handlers_to_call.push_back(wrapper.handler);
                }
            }
        }
    }

    // 在锁外执行回调
    for (const auto& handler : handlers_to_call) {
        try {
            handler(event);
        } catch (const std::exception& e) {
            // 记录异常，避免影响其他处理器
            // TODO: 使用日志系统记录
        } catch (...) {
            // 捕获所有异常
        }
    }
}

SubscriptionId EventBus::next_id() {
    return next_id_.fetch_add(1);
}

} // namespace event
END_NAMESPACE_COMMON
