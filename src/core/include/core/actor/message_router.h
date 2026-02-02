#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <chrono>
#include "core/actor/actor_ref.h"
#include "core/actor/message_router_events.h"
#include "common/event/event_bus.h"
#include "common/metrics/metrics_collector.h"

BEGIN_NAMESPACE_CORE

/**
 * @brief 消息路由器
 *
 * 支持基于规则的消息路由，根据发送者、消息内容等条件
 * 将消息转发到目标 Actor。
 */
class MessageRouter {
public:
    /**
     * @brief 路由规则类型
     */
    using RouteRule = std::function<bool(const ActorRef&, const std::string&)>;

    /**
     * @brief 路由目标
     */
    struct RouteTarget {
        ActorRef actor;              // 目标 Actor
        std::string rule_name;       // 规则名称
        int priority;                // 优先级 (数值越大优先级越高)
        bool enabled;                // 是否启用

        RouteTarget()
            : priority(0)
            , enabled(true) {}
    };

    /**
     * @brief 路由匹配结果
     */
    struct RouteResult {
        ActorRef target;             // 匹配的目标 Actor
        std::string rule_name;       // 匹配的规则名称
        bool matched;                // 是否匹配

        RouteResult()
            : matched(false) {}
    };

    /**
     * @brief 路由统计信息
     */
    struct RouteStats {
        uint64_t total_routes;       // 总路由次数
        uint64_t matched_routes;     // 匹配成功次数
        uint64_t unmatched_routes;   // 未匹配次数
        uint64_t disabled_routes;    // 禁用规则跳过次数

        RouteStats()
            : total_routes(0)
            , matched_routes(0)
            , unmatched_routes(0)
            , disabled_routes(0) {}
    };

public:
    /**
     * @brief 构造函数
     * @param io IoContext 引用（用于事件总线）
     * @param enable_metrics 是否启用指标收集
     */
    explicit MessageRouter(Rendu::io::IoContext& io, bool enable_metrics = true);
    ~MessageRouter() = default;

    /**
     * @brief 添加路由规则
     *
     * @param rule_name 规则名称（必须唯一）
     * @param rule 路由规则函数，返回 true 表示匹配
     * @param target 目标 Actor
     * @param priority 优先级（数值越大优先级越高，默认 0）
     * @return true 添加成功，false 规则名称已存在
     */
    bool add_route(const std::string& rule_name,
                  RouteRule rule,
                  const ActorRef& target,
                  int priority = 0);

    /**
     * @brief 移除路由规则
     *
     * @param rule_name 规则名称
     * @return true 移除成功，false 规则不存在
     */
    bool remove_route(const std::string& rule_name);

    /**
     * @brief 启用路由规则
     *
     * @param rule_name 规则名称
     * @return true 启用成功，false 规则不存在
     */
    bool enable_route(const std::string& rule_name);

    /**
     * @brief 禁用路由规则
     *
     * @param rule_name 规则名称
     * @return true 禁用成功，false 规则不存在
     */
    bool disable_route(const std::string& rule_name);

    /**
     * @brief 设置路由规则优先级
     *
     * @param rule_name 规则名称
     * @param priority 新优先级
     * @return true 设置成功，false 规则不存在
     */
    bool set_route_priority(const std::string& rule_name, int priority);

    /**
     * @brief 路由消息
     *
     * 按优先级顺序匹配规则，返回第一个匹配的目标。
     * 如果没有规则匹配，返回未匹配的 RouteResult。
     *
     * @param sender 发送者 Actor
     * @param message_type 消息类型标识
     * @return RouteResult 路由结果
     */
    RouteResult route(const ActorRef& sender, const std::string& message_type);

    /**
     * @brief 批量路由消息
     *
     * 返回所有匹配规则的目标。
     *
     * @param sender 发送者 Actor
     * @param message_type 消息类型标识
     * @return std::vector<RouteResult> 所有匹配的路由结果
     */
    std::vector<RouteResult> route_all(const ActorRef& sender,
                                     const std::string& message_type);

    /**
     * @brief 广播消息
     *
     * 将消息发送给所有启用的目标。
     *
     * @param message_type 消息类型标识
     * @return std::vector<ActorRef> 所有启用的目标 Actor
     */
    std::vector<ActorRef> broadcast(const std::string& message_type);

    /**
     * @brief 获取路由统计信息
     *
     * @return RouteStats 路由统计
     */
    RouteStats get_stats() const;

    /**
     * @brief 重置路由统计信息
     */
    void reset_stats();

    /**
     * @brief 获取所有路由规则
     *
     * @return std::vector<RouteTarget> 所有路由规则
     */
    std::vector<RouteTarget> get_all_routes() const;

    /**
     * @brief 获取指定路由规则
     *
     * @param rule_name 规则名称
     * @return std::optional<RouteTarget> 路由规则（不存在则返回空）
     */
    std::optional<RouteTarget> get_route(const std::string& rule_name) const;

    /**
     * @brief 清空所有路由规则
     */
    void clear();

    /**
     * @brief 获取路由规则数量
     *
     * @return size_t 路由规则数量
     */
    size_t get_route_count() const;

    /**
     * @brief 检查路由规则是否存在
     *
     * @param rule_name 规则名称
     * @return true 规则存在，false 规则不存在
     */
    bool has_route(const std::string& rule_name) const;

    /**
     * @brief 检查路由规则是否启用
     *
     * @param rule_name 规则名称
     * @return true 规则启用，false 规则不存在或已禁用
     */
    bool is_route_enabled(const std::string& rule_name) const;

    /**
     * @brief 获取事件总线（用于订阅路由事件）
     * @return EventBus& 事件总线引用
     */
    Rendu::event::EventBus& event_bus() { return event_bus_; }

    /**
     * @brief 检查是否启用指标收集
     * @return bool 是否启用
     */
    bool metrics_enabled() const { return enable_metrics_; }

    /**
     * @brief 获取路由指标（Prometheus 格式）
     * @return std::string 指标文本
     */
    std::string get_metrics() const;

private:
    /**
     * @brief 路由规则内部结构
     */
    struct RouteInternal {
        RouteTarget target;
        RouteRule rule;
        size_t match_count;          // 匹配次数统计
        size_t route_count;          // 路由次数统计

        RouteInternal()
            : match_count(0)
            , route_count(0) {}
    };

    /**
     * @brief 记录指标（带标签）
     */
    void record_metric(const std::string& name, double value,
                      const std::map<std::string, std::string>& tags = {});

    bool enable_metrics_;            // 是否启用指标收集
    mutable std::mutex mutex_;
    std::unordered_map<std::string, RouteInternal> routes_;

    // 事件总线（发布路由事件）
    Rendu::event::EventBus event_bus_;

    // 路由统计（使用原子操作保证线程安全）
    mutable std::atomic<uint64_t> total_routes_{0};
    mutable std::atomic<uint64_t> matched_routes_{0};
    mutable std::atomic<uint64_t> unmatched_routes_{0};
    mutable std::atomic<uint64_t> disabled_routes_{0};
};

END_NAMESPACE_CORE
