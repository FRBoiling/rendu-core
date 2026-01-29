#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include "common/event/event.h"
#include "common/event/event_bus.h"
#include "common/event/handler.h"

using namespace Rendu;

// ========== 自定义事件定义 ==========

/// 用户登录事件
class UserLoginEvent : public event::TypedEvent<UserLoginEvent> {
public:
    UserLoginEvent(int32_t user_id, const std::string& username)
        : user_id_(user_id), username_(username) {}

    int32_t user_id() const { return user_id_; }
    const std::string& username() const { return username_; }

private:
    int32_t user_id_;
    std::string username_;
};

/// 用户登出事件
class UserLogoutEvent : public event::TypedEvent<UserLogoutEvent> {
public:
    explicit UserLogoutEvent(int32_t user_id) : user_id_(user_id) {}

    int32_t user_id() const { return user_id_; }

private:
    int32_t user_id_;
};

/// 订单创建事件
class OrderCreatedEvent : public event::TypedEvent<OrderCreatedEvent> {
public:
    OrderCreatedEvent(const std::string& order_id, double amount)
        : order_id_(order_id), amount_(amount) {}

    const std::string& order_id() const { return order_id_; }
    double amount() const { return amount_; }

private:
    std::string order_id_;
    double amount_;
};

/// 系统关闭事件（用于优雅关闭）
class SystemShutdownEvent : public event::TypedEvent<SystemShutdownEvent> {
public:
    SystemShutdownEvent() = default;
};

// ========== 事件处理器 ==========

/// 统计处理器：统计事件数量
struct StatisticsHandler {
    int32_t login_count_{0};
    int32_t logout_count_{0};
    int32_t order_count_{0};

    int32_t login_count() const { return login_count_; }
    int32_t logout_count() const { return logout_count_; }
    int32_t order_count() const { return order_count_; }
};

/// 通知处理器：发送通知
struct NotificationHandler {
    // 无状态
};

/// 审计处理器：记录审计日志
struct AuditHandler {
    // 无状态
};

int main() {
    std::cout << "========== 事件 示例程序 ==========" << std::endl;
    std::cout << std::endl;

    // 创建 IO Context
    io::IoContext io(1);

    // 创建事件总线
    event::EventBus bus(io);
    std::cout << "[总线] 事件总线已创建" << std::endl;

    // 创建处理器状态
    auto stats = std::make_shared<StatisticsHandler>();
    auto notify = std::make_shared<NotificationHandler>();
    auto audit = std::make_shared<AuditHandler>();

    // 注册统计处理器
    auto stats_login_handler = event::make_handler<UserLoginEvent>([stats](const UserLoginEvent& e) {
        stats->login_count_++;
        std::cout << "[统计] 用户登录 - ID: " << e.user_id()
                  << ", 用户名: " << e.username()
                  << " (累计: " << stats->login_count_ << ")" << std::endl;
    });
    auto stats_logout_handler = event::make_handler<UserLogoutEvent>([stats](const UserLogoutEvent& e) {
        stats->logout_count_++;
        std::cout << "[统计] 用户登出 - ID: " << e.user_id()
                  << " (累计: " << stats->logout_count_ << ")" << std::endl;
    });
    auto stats_order_handler = event::make_handler<OrderCreatedEvent>([stats](const OrderCreatedEvent& e) {
        stats->order_count_++;
        std::cout << "[统计] 订单创建 - ID: " << e.order_id()
                  << ", 金额: " << e.amount()
                  << " (累计: " << stats->order_count_ << ")" << std::endl;
    });

    auto stats_login_id = bus.subscribe<UserLoginEvent>(stats_login_handler);
    auto stats_logout_id = bus.subscribe<UserLogoutEvent>(stats_logout_handler);
    auto stats_order_id = bus.subscribe<OrderCreatedEvent>(stats_order_handler);

    // 注册通知处理器
    auto notify_login_handler = event::make_handler<UserLoginEvent>([](const UserLoginEvent& e) {
        std::cout << "[通知] 欢迎用户 " << e.username() << " 登录系统！" << std::endl;
    });
    auto notify_logout_handler = event::make_handler<UserLogoutEvent>([](const UserLogoutEvent& e) {
        std::cout << "[通知] 用户 ID " << e.user_id() << " 已登出，期待再次光临！" << std::endl;
    });

    bus.subscribe<UserLoginEvent>(notify_login_handler);
    bus.subscribe<UserLogoutEvent>(notify_logout_handler);

    // 注册审计处理器
    auto audit_login_handler = event::make_handler<UserLoginEvent>([](const UserLoginEvent& e) {
        std::cout << "[审计] [LOGIN] user_id=" << e.user_id()
                  << ", username=" << e.username() << std::endl;
    });
    auto audit_logout_handler = event::make_handler<UserLogoutEvent>([](const UserLogoutEvent& e) {
        std::cout << "[审计] [LOGOUT] user_id=" << e.user_id() << std::endl;
    });
    auto audit_order_handler = event::make_handler<OrderCreatedEvent>([](const OrderCreatedEvent& e) {
        std::cout << "[审计] [ORDER] order_id=" << e.order_id()
                  << ", amount=" << e.amount() << std::endl;
    });

    bus.subscribe<UserLoginEvent>(audit_login_handler);
    bus.subscribe<UserLogoutEvent>(audit_logout_handler);
    bus.subscribe<OrderCreatedEvent>(audit_order_handler);

    std::cout << "[总线] 已注册处理器" << std::endl;
    std::cout << std::endl;

    std::cout << "=== 发布同步事件 ===" << std::endl;

    // 发布用户登录事件
    UserLoginEvent login_evt1(1001, "Alice");
    bus.publish(login_evt1);

    UserLoginEvent login_evt2(1002, "Bob");
    bus.publish(login_evt2);

    // 发布订单创建事件
    OrderCreatedEvent order_evt1("ORD-001", 99.99);
    bus.publish(order_evt1);

    // 发布用户登出事件
    UserLogoutEvent logout_evt1(1001);
    bus.publish(logout_evt1);

    std::cout << std::endl;
    std::cout << "=== 发布异步事件 ===" << std::endl;

    // 异步发布事件
    bus.publish_async(UserLoginEvent(1003, "Charlie"));
    bus.publish_async(OrderCreatedEvent("ORD-002", 199.99));
    bus.publish_async(UserLoginEvent(1004, "David"));

    // 等待异步事件处理完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << std::endl;
    std::cout << "=== 统计信息 ===" << std::endl;
    std::cout << "登录次数: " << stats->login_count() << std::endl;
    std::cout << "登出次数: " << stats->logout_count() << std::endl;
    std::cout << "订单数量: " << stats->order_count() << std::endl;

    std::cout << std::endl;
    std::cout << "=== 演示取消订阅 ===" << std::endl;

    // 取消登录统计处理器的订阅
    bus.unsubscribe(stats_login_id);
    std::cout << "[总线] 登录统计处理器已取消订阅" << std::endl;

    // 再次发布登录事件（不会触发登录统计，但仍会触发通知和审计）
    bus.publish(UserLoginEvent(1005, "Eve"));

    std::cout << std::endl;
    std::cout << "=== 演示事件类型查询 ===" << std::endl;

    std::cout << "UserLoginEvent 类型: " << UserLoginEvent::static_type() << std::endl;
    std::cout << "OrderCreatedEvent 类型: " << OrderCreatedEvent::static_type() << std::endl;

    std::cout << std::endl;
    std::cout << "========== 示例程序结束 ==========" << std::endl;

    return 0;
}
