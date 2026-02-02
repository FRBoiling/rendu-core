#pragma once

#include "common/event/event.h"
#include "core/actor/actor_ref.h"
#include <string>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>

BEGIN_NAMESPACE_CORE
namespace actor {

/**
 * @brief 获取当前时间戳字符串
 */
inline std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm;
    // 使用线程安全的 localtime_r (POSIX) 或 localtime_s (Windows)
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    // 验证 tm 结构的有效性
    if (tm.tm_year < 0 || tm.tm_year > 3000) {
        return "1970-01-01 00:00:00.000";
    }

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (tm.tm_year + 1900) << "-"
        << std::setw(2) << (tm.tm_mon + 1) << "-"
        << std::setw(2) << tm.tm_mday << " "
        << std::setw(2) << tm.tm_hour << ":"
        << std::setw(2) << tm.tm_min << ":"
        << std::setw(2) << tm.tm_sec << "."
        << std::setw(3) << ms.count();
    return oss.str();
}

/**
 * @brief 路由添加事件
 */
struct RouteAddedEvent : public Rendu::event::TypedEvent<RouteAddedEvent> {
    std::string rule_name;
    int priority;
    Rendu::ActorRef target;
    std::string timestamp;

    RouteAddedEvent(std::string name, int prio, const Rendu::ActorRef& tgt)
        : rule_name(std::move(name))
        , priority(prio)
        , target(tgt)
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 路由移除事件
 */
struct RouteRemovedEvent : public Rendu::event::TypedEvent<RouteRemovedEvent> {
    std::string rule_name;
    std::string timestamp;

    explicit RouteRemovedEvent(std::string name)
        : rule_name(std::move(name))
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 路由启用事件
 */
struct RouteEnabledEvent : public Rendu::event::TypedEvent<RouteEnabledEvent> {
    std::string rule_name;
    std::string timestamp;

    explicit RouteEnabledEvent(std::string name)
        : rule_name(std::move(name))
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 路由禁用事件
 */
struct RouteDisabledEvent : public Rendu::event::TypedEvent<RouteDisabledEvent> {
    std::string rule_name;
    std::string timestamp;

    explicit RouteDisabledEvent(std::string name)
        : rule_name(std::move(name))
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 消息路由事件
 */
struct MessageRoutedEvent : public Rendu::event::TypedEvent<MessageRoutedEvent> {
    Rendu::ActorRef sender;
    Rendu::ActorRef target;
    std::string message_type;
    std::string matched_rule;
    bool success;
    std::string timestamp;
    double duration_ms;  // 路由耗时

    MessageRoutedEvent(const Rendu::ActorRef& s, const Rendu::ActorRef& t,
                      std::string msg_type, std::string rule, bool suc, double dur)
        : sender(s)
        , target(t)
        , message_type(std::move(msg_type))
        , matched_rule(std::move(rule))
        , success(suc)
        , timestamp(get_timestamp())
        , duration_ms(dur) {}
};

/**
 * @brief 路由异常事件
 */
struct RouteExceptionEvent : public Rendu::event::TypedEvent<RouteExceptionEvent> {
    std::string rule_name;
    std::string error_message;
    Rendu::ActorRef sender;
    std::string message_type;
    std::string timestamp;

    RouteExceptionEvent(std::string rule, std::string error,
                       const Rendu::ActorRef& s, std::string msg_type)
        : rule_name(std::move(rule))
        , error_message(std::move(error))
        , sender(s)
        , message_type(std::move(msg_type))
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 批量路由事件
 */
struct BatchRoutedEvent : public Rendu::event::TypedEvent<BatchRoutedEvent> {
    Rendu::ActorRef sender;
    std::string message_type;
    size_t matched_count;
    size_t total_count;
    std::string timestamp;

    BatchRoutedEvent(const Rendu::ActorRef& s, std::string msg_type,
                    size_t matched, size_t total)
        : sender(s)
        , message_type(std::move(msg_type))
        , matched_count(matched)
        , total_count(total)
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 广播事件
 */
struct BroadcastEvent : public Rendu::event::TypedEvent<BroadcastEvent> {
    std::string message_type;
    size_t target_count;
    std::string timestamp;

    BroadcastEvent(std::string msg_type, size_t count)
        : message_type(std::move(msg_type))
        , target_count(count)
        , timestamp(get_timestamp()) {}
};

/**
 * @brief 路由统计重置事件
 */
struct StatsResetEvent : public Rendu::event::TypedEvent<StatsResetEvent> {
    std::string timestamp;

    StatsResetEvent()
        : timestamp(get_timestamp()) {}
};

/**
 * @brief 路由清空事件
 */
struct RoutesClearedEvent : public Rendu::event::TypedEvent<RoutesClearedEvent> {
    size_t cleared_count;
    std::string timestamp;

    explicit RoutesClearedEvent(size_t count)
        : cleared_count(count)
        , timestamp(get_timestamp()) {}
};

} // namespace actor
END_NAMESPACE_CORE
