//**********************************
//  Created by boil on 2026/1/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/event/event_bus.h"
#include "common/event/event.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace Rendu;
using namespace Rendu::event;
using namespace Rendu::io;
using namespace Rendu::log;

// 测试辅助：为日志系统创建一个简单的 IoContext
struct TestLoggerSetup {
    IoContext io;
    std::thread io_thread;

    TestLoggerSetup() : io(1) {
        init_default_io_context(io);
        // 在单独的线程中运行 io_context
        io_thread = std::thread([this]() {
            io.run();
        });
    }

    ~TestLoggerSetup() {
        io.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }
};

// 全局日志初始化（每个测试文件一个）
static TestLoggerSetup g_logger_setup;

// 测试事件类型
class IntEvent : public TypedEvent<IntEvent> {
public:
    int value = 0;
    IntEvent() = default;
    explicit IntEvent(int v) : value(v) {}
};

class StringEvent : public TypedEvent<StringEvent> {
public:
    std::string message;
    StringEvent() = default;
    explicit StringEvent(const std::string& msg) : message(msg) {}
};

TEST_CASE("EventBus: 订阅和发布", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};
    std::atomic<int> last_value{0};

    bus.subscribe<IntEvent>([&count, &last_value](const Event& e) {
        const IntEvent* ie = dynamic_cast<const IntEvent*>(&e);
        if (ie) {
            count++;
            last_value = ie->value;
        }
    });

    IntEvent event(42);
    bus.publish(event);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(count == 1);
    REQUIRE(last_value == 42);

    io_ctx.stop();
}

TEST_CASE("EventBus: 多个订阅者", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count1{0};
    std::atomic<int> count2{0};
    std::atomic<int> count3{0};

    bus.subscribe<IntEvent>([&count1](const Event&) { count1++; });
    bus.subscribe<IntEvent>([&count2](const Event&) { count2++; });
    bus.subscribe<IntEvent>([&count3](const Event&) { count3++; });

    bus.publish(IntEvent(1));
    bus.publish(IntEvent(2));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(count1 == 2);
    REQUIRE(count2 == 2);
    REQUIRE(count3 == 2);

    io_ctx.stop();
}

TEST_CASE("EventBus: 异步发布", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<bool> executed{false};
    std::atomic<int> value{0};

    bus.subscribe<IntEvent>([&executed, &value](const Event& e) {
        const IntEvent* ie = dynamic_cast<const IntEvent*>(&e);
        if (ie) {
            executed = true;
            value = ie->value;
        }
    });

    IntEvent event(99);
    bus.publish_async(event);

    REQUIRE(executed == false); // 立即检查应该是 false

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    REQUIRE(executed == true);
    REQUIRE(value == 99);

    io_ctx.stop();
}

TEST_CASE("EventBus: 延迟发布", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<bool> executed{false};
    std::atomic<int> value{0};

    bus.subscribe<IntEvent>([&executed, &value](const Event& e) {
        const IntEvent* ie = dynamic_cast<const IntEvent*>(&e);
        if (ie) {
            executed = true;
            value = ie->value;
        }
    });

    IntEvent event(123);
    auto start = std::chrono::steady_clock::now();
    bus.publish_delayed(event, 100); // 延迟 100ms

    REQUIRE(executed == false);

    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    REQUIRE(executed == true);
    REQUIRE(value == 123);
    REQUIRE(duration.count() >= 90); // 至少 90ms

    io_ctx.stop();
}

TEST_CASE("EventBus: 取消订阅", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    auto id = bus.subscribe<IntEvent>([&count](const Event&) { count++; });

    bus.publish(IntEvent(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    REQUIRE(count == 1);

    bus.unsubscribe(id);
    bus.publish(IntEvent(2));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    REQUIRE(count == 1); // 没有增加

    io_ctx.stop();
}

TEST_CASE("EventBus: 优先级", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::vector<int> order;

    bus.subscribe_with_priority(IntEvent::static_type(),
        [&order](const Event&) { order.push_back(1); }, 1);

    bus.subscribe_with_priority(IntEvent::static_type(),
        [&order](const Event&) { order.push_back(3); }, 3);

    bus.subscribe_with_priority(IntEvent::static_type(),
        [&order](const Event&) { order.push_back(2); }, 2);

    bus.publish(IntEvent(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 3); // 优先级最高的先执行
    REQUIRE(order[1] == 2);
    REQUIRE(order[2] == 1);

    io_ctx.stop();
}

TEST_CASE("EventBus: 不同事件类型", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> int_count{0};
    std::atomic<int> string_count{0};

    bus.subscribe<IntEvent>([&int_count](const Event&) { int_count++; });
    bus.subscribe<StringEvent>([&string_count](const Event&) { string_count++; });

    bus.publish(IntEvent(42));
    bus.publish(StringEvent("hello"));
    bus.publish(IntEvent(99));
    bus.publish(StringEvent("world"));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(int_count == 2);
    REQUIRE(string_count == 2);

    io_ctx.stop();
}

TEST_CASE("EventBus: 订阅者计数", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);

    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 0);

    bus.subscribe<IntEvent>([](const Event&) {});
    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 1);

    bus.subscribe<IntEvent>([](const Event&) {});
    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 2);

    auto id = bus.subscribe<IntEvent>([](const Event&) {});
    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 3);

    bus.unsubscribe(id);
    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 2);

    io_ctx.stop();
}

TEST_CASE("EventBus: 清空", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    bus.subscribe<IntEvent>([&count](const Event&) { count++; });
    bus.subscribe<StringEvent>([&count](const Event&) { count++; });

    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 1);
    REQUIRE(bus.subscriber_count(StringEvent::static_type()) == 1);

    bus.clear();

    REQUIRE(bus.subscriber_count(IntEvent::static_type()) == 0);
    REQUIRE(bus.subscriber_count(StringEvent::static_type()) == 0);

    bus.publish(IntEvent(1));
    bus.publish(StringEvent("test"));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    REQUIRE(count == 0); // 没有处理器执行

    io_ctx.stop();
}

TEST_CASE("EventBus: 异常处理", "[event][bus]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    bus.subscribe<IntEvent>([&count](const Event&) {
        count++;
        throw std::runtime_error("test exception");
    });

    bus.subscribe<IntEvent>([&count](const Event&) {
        count++;
    });

    // 即使第一个处理器抛出异常，第二个也应该执行
    bus.publish(IntEvent(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(count == 2); // 两个处理器都执行了

    io_ctx.stop();
}

TEST_CASE("EventBus: 高吞吐量", "[event][bus]") {
    IoContext io_ctx(2);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    bus.subscribe<IntEvent>([&count](const Event&) { count++; });

    const int num_events = 10000;
    for (int i = 0; i < num_events; ++i) {
        bus.publish(IntEvent(i));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    REQUIRE(count == num_events);

    io_ctx.stop();
}

TEST_CASE("EventBus: 并发订阅和发布", "[event][bus]") {
    IoContext io_ctx(4);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    // 并发订阅
    const int num_subscribers = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_subscribers; ++i) {
        threads.emplace_back([&bus, &count]() {
            bus.subscribe<IntEvent>([&count](const Event&) { count++; });
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // 并发发布
    const int num_events = 1000;
    threads.clear();

    for (int i = 0; i < num_subscribers; ++i) {
        threads.emplace_back([&bus, num_events, i]() {
            for (int j = 0; j < num_events / num_subscribers; ++j) {
                bus.publish(IntEvent(j));
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 10 个订阅者 × 1000 个事件 = 10000 次触发
    REQUIRE(count == num_events * num_subscribers);

    io_ctx.stop();
}
