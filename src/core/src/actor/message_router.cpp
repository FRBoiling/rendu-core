#include "core/actor/message_router.h"
#include "core/actor/message_router_events.h"
#include "common/metrics/metrics_collector.h"

#include <algorithm>

BEGIN_NAMESPACE_CORE

MessageRouter::MessageRouter(Rendu::io::IoContext& io, bool enable_metrics)
    : enable_metrics_(enable_metrics)
    , event_bus_(io) {

    // 设置指标元数据
    if (enable_metrics_) {
        auto& mc = Rendu::metrics::MetricsCollector::instance();
        mc.set_metadata("message_router_add_route_total",
                       "Total number of routes added to the message router", "counter");
        mc.set_metadata("message_router_remove_route_total",
                       "Total number of routes removed from the message router", "counter");
        mc.set_metadata("message_router_enable_route_total",
                       "Total number of routes enabled", "counter");
        mc.set_metadata("message_router_disable_route_total",
                       "Total number of routes disabled", "counter");
        mc.set_metadata("message_router_set_priority_total",
                       "Total number of route priority changes", "counter");
        mc.set_metadata("message_router_route_total",
                       "Total number of message routing operations", "counter");
        mc.set_metadata("message_router_route_duration_ms",
                       "Duration of message routing operations in milliseconds", "histogram");
        mc.set_metadata("message_router_route_all_total",
                       "Total number of batch message routing operations", "counter");
        mc.set_metadata("message_router_broadcast_total",
                       "Total number of message broadcasts", "counter");
        mc.set_metadata("message_router_clear_total",
                       "Total number of route clear operations", "counter");
    }
}

void MessageRouter::record_metric(const std::string& name, double value,
                                 const std::map<std::string, std::string>& tags) {
    if (!enable_metrics_) {
        return;
    }
    Rendu::metrics::MetricsCollector::instance().increment_counter(name, value, tags);
}

bool MessageRouter::add_route(const std::string& rule_name,
                             RouteRule rule,
                             const ActorRef& target,
                             int priority) {
    bool route_exists;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        route_exists = (routes_.find(rule_name) != routes_.end());

        if (!route_exists) {
            RouteInternal route;
            route.target.actor = target;
            route.target.rule_name = rule_name;
            route.target.priority = priority;
            route.target.enabled = true;
            route.rule = std::move(rule);
            route.match_count = 0;
            route.route_count = 0;

            routes_[rule_name] = std::move(route);
        }
    }

    // 在锁外发布事件和记录指标
    if (route_exists) {
        record_metric("message_router_add_route_total", 1.0, {{"status", "failed"}});
        event_bus_.publish(actor::RouteRemovedEvent(rule_name));
        return false;
    }

    record_metric("message_router_add_route_total", 1.0, {{"status", "success"}});
    record_metric("message_router_routes_total", 1.0, {{"priority", std::to_string(priority)}});
    event_bus_.publish(actor::RouteAddedEvent(rule_name, priority, target));
    return true;
}

bool MessageRouter::remove_route(const std::string& rule_name) {
    bool removed = false;
    size_t count = 0;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = routes_.find(rule_name);
        if (it != routes_.end()) {
            removed = true;
            count = routes_.size();
            routes_.erase(it);
        }
    }

    if (removed) {
        record_metric("message_router_remove_route_total", 1.0, {{"status", "success"}});
        event_bus_.publish(actor::RouteRemovedEvent(rule_name));
    } else {
        record_metric("message_router_remove_route_total", 1.0, {{"status", "failed"}});
    }

    return removed;
}

bool MessageRouter::enable_route(const std::string& rule_name) {
    bool enabled = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = routes_.find(rule_name);
        if (it != routes_.end()) {
            it->second.target.enabled = true;
            enabled = true;
        }
    }

    if (enabled) {
        record_metric("message_router_enable_route_total", 1.0, {{"status", "success"}});
        event_bus_.publish(actor::RouteEnabledEvent(rule_name));
    } else {
        record_metric("message_router_enable_route_total", 1.0, {{"status", "failed"}});
    }

    return enabled;
}

bool MessageRouter::disable_route(const std::string& rule_name) {
    bool disabled = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = routes_.find(rule_name);
        if (it != routes_.end()) {
            it->second.target.enabled = false;
            disabled = true;
        }
    }

    if (disabled) {
        record_metric("message_router_disable_route_total", 1.0, {{"status", "success"}});
        event_bus_.publish(actor::RouteDisabledEvent(rule_name));
    } else {
        record_metric("message_router_disable_route_total", 1.0, {{"status", "failed"}});
    }

    return disabled;
}

bool MessageRouter::set_route_priority(const std::string& rule_name, int priority) {
    bool set = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = routes_.find(rule_name);
        if (it != routes_.end()) {
            it->second.target.priority = priority;
            set = true;
        }
    }

    if (set) {
        record_metric("message_router_set_priority_total", 1.0, {{"status", "success"}});
    } else {
        record_metric("message_router_set_priority_total", 1.0, {{"status", "failed"}});
    }

    return set;
}

MessageRouter::RouteResult MessageRouter::route(const ActorRef& sender,
                                                const std::string& message_type) {
    total_routes_.fetch_add(1, std::memory_order_relaxed);

    auto start = std::chrono::high_resolution_clock::now();

    RouteResult result;
    std::string matched_rule;
    std::string exception_msg;
    std::string exception_rule;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        // 按优先级排序规则
        std::vector<std::pair<int, std::string>> sorted_rules;
        for (const auto& [name, route] : routes_) {
            if (route.target.enabled) {
                sorted_rules.emplace_back(route.target.priority, name);
            }
        }

        // 按优先级降序排序（数值越大优先级越高）
        std::sort(sorted_rules.begin(), sorted_rules.end(),
                  [](const auto& a, const auto& b) {
                      return a.first > b.first;
                  });

        // 按优先级顺序匹配规则
        for (const auto& [priority, name] : sorted_rules) {
            auto& route = routes_[name];
            route.route_count++;

            try {
                if (route.rule(sender, message_type)) {
                    route.match_count++;
                    matched_routes_.fetch_add(1, std::memory_order_relaxed);

                    result.target = route.target.actor;
                    result.rule_name = name;
                    result.matched = true;
                    matched_rule = name;
                    break;
                }
            } catch (const std::exception& e) {
                exception_rule = name;
                exception_msg = e.what();
            }
        }

        if (!result.matched) {
            unmatched_routes_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // 计算路由耗时
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // 在锁外发布事件和记录指标
    if (result.matched) {
        record_metric("message_router_route_total", 1.0, {{"status", "matched"}});
        record_metric("message_router_route_duration_ms", duration_ms, {{"type", "single"}});
        event_bus_.publish(actor::MessageRoutedEvent(sender, result.target, message_type,
                                                      matched_rule, true, duration_ms));
    } else if (!exception_rule.empty()) {
        record_metric("message_router_route_total", 1.0, {{"status", "exception"}});
        event_bus_.publish(actor::RouteExceptionEvent(exception_rule, exception_msg,
                                                     sender, message_type));
    } else {
        record_metric("message_router_route_total", 1.0, {{"status", "unmatched"}});
    }

    return result;
}

std::vector<MessageRouter::RouteResult> MessageRouter::route_all(
    const ActorRef& sender,
    const std::string& message_type) {
    total_routes_.fetch_add(1, std::memory_order_relaxed);

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::pair<std::string, std::string>> exceptions; // 规则名, 错误消息
    std::vector<RouteResult> results;
    size_t total_route_count = 0;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        total_route_count = routes_.size();

        for (auto& [name, route] : routes_) {
            if (!route.target.enabled) {
                disabled_routes_.fetch_add(1, std::memory_order_relaxed);
                continue;
            }

            route.route_count++;

            try {
                if (route.rule(sender, message_type)) {
                    route.match_count++;
                    matched_routes_.fetch_add(1, std::memory_order_relaxed);

                    RouteResult result;
                    result.target = route.target.actor;
                    result.rule_name = name;
                    result.matched = true;

                    results.push_back(std::move(result));
                }
            } catch (const std::exception& e) {
                exceptions.push_back({name, e.what()});
            }
        }

        if (results.empty()) {
            unmatched_routes_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // 计算路由耗时
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // 在锁外发布事件和记录指标
    for (const auto& [rule, msg] : exceptions) {
        record_metric("message_router_route_total", 1.0, {{"status", "exception"}});
        event_bus_.publish(actor::RouteExceptionEvent(rule, msg, sender, message_type));
    }

    record_metric("message_router_route_all_total", 1.0, {{"matched_count", std::to_string(results.size())}});
    record_metric("message_router_route_duration_ms", duration_ms, {{"type", "batch"}});
    event_bus_.publish(actor::BatchRoutedEvent(sender, message_type, results.size(), total_route_count));

    return results;
}

std::vector<ActorRef> MessageRouter::broadcast(const std::string& message_type) {
    std::vector<ActorRef> targets;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [name, route] : routes_) {
            if (route.target.enabled) {
                targets.push_back(route.target.actor);
            }
        }
    }

    record_metric("message_router_broadcast_total", 1.0, {{"target_count", std::to_string(targets.size())}});
    event_bus_.publish(actor::BroadcastEvent(message_type, targets.size()));

    return targets;
}

MessageRouter::RouteStats MessageRouter::get_stats() const {
    RouteStats stats;
    stats.total_routes = total_routes_.load(std::memory_order_relaxed);
    stats.matched_routes = matched_routes_.load(std::memory_order_relaxed);
    stats.unmatched_routes = unmatched_routes_.load(std::memory_order_relaxed);
    stats.disabled_routes = disabled_routes_.load(std::memory_order_relaxed);

    return stats;
}

void MessageRouter::reset_stats() {
    total_routes_.store(0, std::memory_order_relaxed);
    matched_routes_.store(0, std::memory_order_relaxed);
    unmatched_routes_.store(0, std::memory_order_relaxed);
    disabled_routes_.store(0, std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [name, route] : routes_) {
            route.match_count = 0;
            route.route_count = 0;
        }
    }

    event_bus_.publish(actor::StatsResetEvent());
}

std::vector<MessageRouter::RouteTarget> MessageRouter::get_all_routes() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<RouteTarget> routes;
    routes.reserve(routes_.size());

    for (const auto& [name, route] : routes_) {
        routes.push_back(route.target);
    }

    return routes;
}

std::optional<MessageRouter::RouteTarget> MessageRouter::get_route(
    const std::string& rule_name) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = routes_.find(rule_name);
    if (it == routes_.end()) {
        return std::nullopt;
    }

    return it->second.target;
}

void MessageRouter::clear() {
    size_t count;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        count = routes_.size();
        routes_.clear();
    }

    record_metric("message_router_clear_total", 1.0, {{"cleared_count", std::to_string(count)}});
    event_bus_.publish(actor::RoutesClearedEvent(count));
}

std::string MessageRouter::get_metrics() const {
    return Rendu::metrics::MetricsCollector::instance().export_metrics("prometheus");
}

size_t MessageRouter::get_route_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return routes_.size();
}

bool MessageRouter::has_route(const std::string& rule_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return routes_.find(rule_name) != routes_.end();
}

bool MessageRouter::is_route_enabled(const std::string& rule_name) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = routes_.find(rule_name);
    if (it == routes_.end()) {
        return false;
    }

    return it->second.target.enabled;
}

END_NAMESPACE_CORE
