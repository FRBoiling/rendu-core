//**********************************
//  Created by boil on 2026/1/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/event/handler.h"
#include "common/event/event_bus.h"
#include "common/event/event.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <atomic>
#include <thread>
#include <chrono>

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

TEST_CASE("Handler: 基本使用", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> received_value{0};

    Handler<IntEvent> handler([&received_value](const IntEvent& e) {
        received_value = e.value;
    });

    bus.subscribe<IntEvent>(handler);

    IntEvent event(42);
    bus.publish(event);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(received_value == 42);

    io_ctx.stop();
}

TEST_CASE("Handler: make_helper", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    auto handler = make_handler<IntEvent>([&count](const IntEvent& e) {
        count += e.value;
    });

    bus.subscribe<IntEvent>(handler);

    bus.publish(IntEvent(10));
    bus.publish(IntEvent(20));
    bus.publish(IntEvent(30));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(count == 60);

    io_ctx.stop();
}

TEST_CASE("Handler: 错误的事件类型", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> int_count{0};
    std::atomic<int> string_count{0};

    // 订阅 IntEvent
    Handler<IntEvent> int_handler([&int_count](const IntEvent& e) {
        int_count += e.value;
    });

    // 订阅 StringEvent
    Handler<StringEvent> string_handler([&string_count](const StringEvent& e) {
        string_count += static_cast<int>(e.message.length());
    });

    bus.subscribe<IntEvent>(int_handler);
    bus.subscribe<StringEvent>(string_handler);

    // 发布 IntEvent，不应该触发 StringEvent 的 handler
    bus.publish(IntEvent(100));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(int_count == 100);
    REQUIRE(string_count == 0);

    io_ctx.stop();
}

TEST_CASE("Handler: 同一类型的多个处理器", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> sum{0};
    std::atomic<int> product{1};

    Handler<IntEvent> sum_handler([&sum](const IntEvent& e) {
        sum += e.value;
    });

    Handler<IntEvent> product_handler([&product](const IntEvent& e) {
        product.store(product.load() * e.value);
    });

    bus.subscribe<IntEvent>(sum_handler);
    bus.subscribe<IntEvent>(product_handler);

    bus.publish(IntEvent(2));
    bus.publish(IntEvent(3));
    bus.publish(IntEvent(5));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(sum == 10);   // 2 + 3 + 5
    REQUIRE(product == 30); // 2 * 3 * 5

    io_ctx.stop();
}

TEST_CASE("Handler: 引用捕获", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::vector<int> values;

    // 使用 lambda 捕获外部变量
    Handler<IntEvent> handler([&values](const IntEvent& e) {
        values.push_back(e.value);
    });

    bus.subscribe<IntEvent>(handler);

    bus.publish(IntEvent(1));
    bus.publish(IntEvent(2));
    bus.publish(IntEvent(3));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(values.size() == 3);
    REQUIRE(values[0] == 1);
    REQUIRE(values[1] == 2);
    REQUIRE(values[2] == 3);

    io_ctx.stop();
}

TEST_CASE("Handler: 与 EventBus 模板订阅", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::atomic<int> count{0};

    // 直接使用 lambda 订阅
    bus.subscribe<IntEvent>([&count](const Event& e) {
        const IntEvent* ie = dynamic_cast<const IntEvent*>(&e);
        if (ie) {
            count += ie->value;
        }
    });

    // 使用 Handler 订阅
    Handler<IntEvent> handler([&count](const IntEvent& e) {
        count.store(count.load() * 2);
    });
    bus.subscribe<IntEvent>(handler);

    bus.publish(IntEvent(5));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // 第一个处理器：count = 5
    // 第二个处理器：count = 5 * 2 = 10
    REQUIRE(count == 10);

    io_ctx.stop();
}

TEST_CASE("Handler: 字符串事件", "[event][handler]") {
    IoContext io_ctx(1);
    std::thread([&io_ctx]() { io_ctx.run(); }).detach();

    EventBus bus(io_ctx);
    std::vector<std::string> messages;

    Handler<StringEvent> handler([&messages](const StringEvent& e) {
        messages.push_back(e.message);
    });

    bus.subscribe<StringEvent>(handler);

    bus.publish(StringEvent("hello"));
    bus.publish(StringEvent("world"));
    bus.publish(StringEvent("rendu"));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    REQUIRE(messages.size() == 3);
    REQUIRE(messages[0] == "hello");
    REQUIRE(messages[1] == "world");
    REQUIRE(messages[2] == "rendu");

    io_ctx.stop();
}
